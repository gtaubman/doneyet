#include <math.h>
#include "hierarchical-list.h"
#include "dialog-box.h"
#include "utils.h"

ListItem::ListItem()
  : height_(1),
    index_(-1),
    depth_(-1) {
  should_expand_ = true;
}

ListItem::~ListItem() {
  // Nothing to delete at the moment.
}

int ListItem::ComputeHeight(int width, string& prepend) {
  int lines_used = 1;
  string text = prepend + Text();
  int curx = 0;
  for (int i = 0; i < text.size(); ++i) {
    int chars_to_ws = chars_to_whitespace(text, i);

    // Make sure to keep this line in sync with the equivalent line in Draw()
    if (curx + chars_to_ws > width) {
      ++lines_used;
      curx = prepend.length();
    }
    ++curx;
  }

  return lines_used;
}

HierarchicalList::HierarchicalList(string& name,
    int height,
    int width,
    int y,
    int x) {
  // Create the frills window
  win_ = newwin(height, width, y, x);

  // Create the window that actual holds the text.  We need to leave three lines
  // at the top: one for the border, one for the label, one for the line under
  // the label.  We also need to leave three on the right.  Two for the scrollbar
  // and one for the righthand border.  Also add one on the left for the line
  // indicator.
  subwin_ = derwin(win_, height - 3, width - 3, 3, 1);
  top_line_ = 0;
  selected_line_ = -1;
  win_rel_selected_line_ = -1;
  total_lines_ = 0;
  indent_ = 2;
  name_ = name;
  selected_item_ = NULL;
  prepend_ = "- ";
  flush_left_text_border_ = true;
  prepend_size_ = prepend_.length();
}

HierarchicalList::~HierarchicalList() {
  delwin(subwin_);
  delwin(win_);
}

void HierarchicalList::SetDatasource(HierarchicalListDataSource* d) {
  datasource_ = d;
  UpdateFlattenedItems();
}

void HierarchicalList::Draw() {
  UpdateFlattenedItems();
  // First figure out how big our sub-window is:
  window_info sinfo = get_window_info(subwin_);
  int swidth = sinfo.width;
  int sheight = sinfo.height;

  // Clear the window 'cause who knows what was there before.
  wclear(subwin_);

  int lines_used = -top_line_;
  for (int i = 0; i < NumRoots(); ++i) {
    lines_used += Draw(Root(i), lines_used, 0);
  }

  // Now that we're done drawing text, draw the frills:
  window_info info = get_window_info(win_);
  int width = info.width;
  int height = info.height;

  // Draw the scrollbar
  for (int i = 3; i < sheight + 2; ++i) {
    mvwaddch(win_, i, swidth + 1, ACS_CKBOARD);
  }
  // Draw the block.
  if (total_lines_) {
    mvwaddch(win_, 3 + lrintf(selected_line_ * (winheight(subwin_) - 1) /
          total_lines_), swidth + 1, ACS_BLOCK);
  }
  
  // Draw the title
  print_in_middle(win_, 1, 0, width, name_.c_str(), COLOR_PAIR(2));

  // Draw the border around the frill window
  box(win_, 0, 0);

  // Draw the divider between title and text
  wmove(win_, 2, 0);
  waddch(win_, ACS_LTEE);
  for (int i = 1; i < width - 1; ++i) {
    waddch(win_, ACS_HLINE);
  }
  waddch(win_, ACS_RTEE);

  
  wrefresh(win_);
}

int HierarchicalList::GetInput() {
  UpdateFlattenedItems();
  Draw();

  return wgetch(win_);
}

int HierarchicalList::Draw(ListItem* node, int line_num, int indent) {
  const string text = (node->ShouldExpand() ? prepend_ : string("+ ")) + node->Text();

  window_info info = get_window_info(subwin_);
  wmove(subwin_, line_num, indent);
  int curx = indent;
  int cury = line_num;

  // If we're selected reverse the text
  if (node == selected_item_) {
    wattron(subwin_, A_UNDERLINE | node->Color());
  } else {
    wattron(subwin_, node->Color());
  }
  
  int lines_used = 1;
  for (int i = 0; i < text.size(); ++i) {
    // Figure out how many characters from i to the next white space.
    int chars_to_ws = chars_to_whitespace(text, i);
    if (curx + chars_to_ws > info.width) {
      curx = indent + (flush_left_text_border_ ? prepend_size_ : 0);
      ++cury;
      ++lines_used;
    }
    mvwaddch(subwin_, cury, curx++, text.c_str()[i]);
  }

  // If we're selected, we're done reversing the text.
  if (node == selected_item_) {
    wattroff(subwin_, A_UNDERLINE | node->Color());
  } else {
    wattroff(subwin_, node->Color());
  }

  // now that we've drawn ourselves, draw our children:
  if (node->ShouldExpand()) {
    for (int i = 0; i < node->NumChildren(); ++i) {
      lines_used += Draw(node->Child(i), line_num + lines_used, indent + indent_);
    }
  }

  return lines_used;
}

void HierarchicalList::SelectPrevItem() {
  if (!flattened_items_.size()) {
    return;
  }
  if (selected_item_ == NULL) {
    // Select the bottom item
    SelectItem(flattened_items_.size() - 1, SCROLL_BOTTOM);
  } else if (selected_item_ == flattened_items_[0]) {
    // We're at the top of the list and hit scroll up.  Don't do anything.
    return;
  } else if (win_rel_selected_line_ -
      flattened_items_[selected_item_->Index() - 1]->Height() <= 0) {
    // Either we're already at the top, or the first line of the previous item
    // is off the screen.  Scroll up.
    SelectItem(selected_item_->Index() - 1, SCROLL_TOP);
  } else {
    // It's just a normal case of selecting the previous item.  No window moving
    // necessary.
    SelectItem(selected_item_->Index() - 1, SCROLL_NONE);
  }
}

void HierarchicalList::SelectNextItem() {
  if (!flattened_items_.size()) {
    return;
  }
  if (selected_item_ == NULL) {
    // Nobody's selected.  Select the first guy.
    SelectItem(0, SCROLL_TOP);
  } else if (selected_item_->Index() == flattened_items_.size() - 1) {
    // We just hit next when the last guy was selected.  Don't do anything.
    return;
  } else if (win_rel_selected_line_ + selected_item_->Height() +
      flattened_items_[selected_item_->Index() + 1]->Height() >=
      winheight(subwin_)) {
    // We need to scroll to show the next guy at the bottom.
    SelectItem(selected_item_->Index() + 1, SCROLL_BOTTOM);
  } else {
    // Normal situation.  Select the next guy.
    if (selected_item_->Index() != flattened_items_.size() - 1) {
      SelectItem(selected_item_->Index() + 1, SCROLL_NONE);
    }
  }
}

void HierarchicalList::SelectPrevLine() {
  if (!flattened_items_.size()) {
    return;
  }
  if (selected_item_ == NULL) {
    // Nobody's selected.  Select the last guy.
    SelectItem(flattened_items_.size() - 1);
    if (total_lines_ > winheight(subwin_)) {
      // We need to scroll things to get to the last guy.
      top_line_ = total_lines_ - (winheight(subwin_) - 1);
      win_rel_selected_line_ = winheight(subwin_) - 1;
      selected_line_ = total_lines_;
    } else {
      top_line_ = 0;
      win_rel_selected_line_ = total_lines_;
      selected_line_ = total_lines_;
    }
  }
  
  ListItem* item = ItemForLineNumber(selected_line_ - 1);
  if (item == NULL) {
    // We're at the end of our list of items
    return;
  }

  if (win_rel_selected_line_ == 0) {
    // We're at the top of the window
    --top_line_;
  } else {
    --win_rel_selected_line_;
  }
  --selected_line_;
  selected_item_ = item;
}

void HierarchicalList::SelectNextLine() {
  if (!flattened_items_.size()) {
    return;
  }
  if (selected_item_ == NULL) {
    // Nobody is selected.  Select the first guy.
    top_line_ = 0;
    selected_line_ = -1;
    win_rel_selected_line_ = -1;
  }
  ListItem* item = ItemForLineNumber(selected_line_ + 1);
  if (item == NULL) {
    // We're at the end of our list of items
    return;
  }

  if (win_rel_selected_line_ + 1 == winheight(subwin_) - 1) {
    // We're going to be at the bottom of the window.
    ++top_line_;
  } else {
    ++win_rel_selected_line_;
  }
  ++selected_line_;
  selected_item_ = item;
}

int HierarchicalList::NumLinesDownInList(ListItem* item) {
  int d = 0;
  for (int i = 0; i < flattened_items_.size(); ++i) {
    if (item == flattened_items_[i])
      break;
    d += flattened_items_[i]->Height();
  }

  return d;
}

void HierarchicalList::SelectItem(int item_index, ScrollType type) {
  if (item_index < 0) {
    SelectItem(item_index);
  }

  if (type == SCROLL_TOP) {
    // We're going to show the item at the top of the window.  This is a pretty
    // easy case because regardless of the number of items, we can always do
    // this.
    selected_item_ = flattened_items_[item_index];
    top_line_ = NumLinesDownInList(selected_item_);
    selected_line_ = top_line_;
    win_rel_selected_line_ = 0;
  } else if (type == SCROLL_BOTTOM) {
    // Here things get complicated.  The situation can arise where we don't
    // actually have enough lines of ListItems in order to place the desired
    // item at the bottom of the window.
    if (total_lines_ >= winheight(subwin_)) {
      // We have enough items to put one at the bottom.
      selected_item_ = flattened_items_[item_index];
      top_line_ = NumLinesDownInList(selected_item_) -
        (winheight(subwin_) - selected_item_->Height()) + 1;
      selected_line_ = NumLinesDownInList(selected_item_);
      win_rel_selected_line_ = winheight(subwin_) - selected_item_->Height() - 1;
    } else {
      // We don't have enough items to warrant putting one at the bottom of the
      // screen.  In this case, we act just like a normal NONE scrolltype had
      // been passed.
      type = SCROLL_NONE;
    }
  }

  if (type == SCROLL_NONE) {
    // The reason this is outside the else if block is to allow a SCROLL_BOTTOM
    // to turn into a SCROLL_NONE.
    SelectItem(item_index);
  }
}

void HierarchicalList::SelectItem(int item_index) {
  if (item_index < 0) {
    selected_item_ = NULL;
    selected_line_ -= win_rel_selected_line_;
    win_rel_selected_line_ = 0;
  } else {
    selected_item_ = flattened_items_[item_index];
    win_rel_selected_line_ = NumLinesDownInList(selected_item_) - top_line_;
    selected_line_ = NumLinesDownInList(selected_item_);
  }
}

void HierarchicalList::EditSelectedItem() {
  if (selected_item_ == NULL)
    return;

  string answer = DialogBox::RunMultiLine("Please Edit Task",
      selected_item_->Text(),
      winwidth(win_) / 3,
      winheight(win_) / 3);

  selected_item_->SetText(answer);
}

void HierarchicalList::ToggleExpansionOfSelectedItem() {
  if (selected_item_) {
    selected_item_->ToggleExpanded();

    // Update our flattened items because the task_for_line_ mapping just
    // changed.
    UpdateFlattenedItems();
  }
}

void HierarchicalList::UpdateFlattenedItems() {
  // Do a pre-order traversal of the item tree and add them in that order to our
  // vector.
  flattened_items_.clear();
  total_lines_ = 0;

  for (int i = 0; i < NumRoots(); ++i) {
    PreOrderAddToList(Root(i));
  }

  for (int i = 0; i < flattened_items_.size(); ++i) {
    flattened_items_[i]->SetIndex(i);
  }

  // Get how much space we have to draw in:
  window_info info = get_window_info(subwin_);
  
  // Compute the heights of all of our list items.
  item_for_line_.clear();
  for (int i = 0; i < flattened_items_.size(); ++i) {
    ListItem* item = flattened_items_[i];
    int height = item->ComputeHeight(info.width - item->Depth() * indent_,
                                     prepend_);
    item->SetHeight(height);
    for (int j = 0; j < height; ++j) {
      item_for_line_.push_back(item);
    }
    total_lines_ += item->Height();
  }
}

void HierarchicalList::PreOrderAddToList(ListItem* l) {
  flattened_items_.push_back(l);
  if (l->Parent() != NULL) {
    l->SetDepth(l->Parent()->Depth() + 1);
  } else {
    l->SetDepth(0);
  }

  for (int i = 0; i < l->NumChildren(); ++i) {
    if (l->ShouldExpand()) {
      PreOrderAddToList(l->Child(i));
    }
  }
}

ListItem* HierarchicalList::ItemForLineNumber(int n) {
  if (n < 0 || n >= item_for_line_.size())
    return NULL;

  return item_for_line_[n];
}
