#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include "hierarchical-list.h"
#include "dialog-box.h"
#include "utils.h"

using std::string;

ListItem::ListItem() : height_(1), index_(-1), depth_(-1) {
  should_expand_ = true;
}

ListItem::~ListItem() {
  // Nothing to delete at the moment.
}

HierarchicalList::HierarchicalList(string& name, int height, int width, int y,
                                   int x, const ColumnSpec& spec) {
  // Create the frills window
  height_ = height;
  width_ = width;
  win_ = newwin(height_, width_, y, x);
  top_line_ = 0;
  selected_line_ = -1;
  selected_column_ = 0;
  win_rel_selected_line_ = -1;
  total_lines_ = 0;
  indent_ = 2;
  name_ = name;
  selected_item_ = NULL;
  prepend_ = "- ";
  flush_left_text_border_ = true;
  draw_column_headers_ = true;
  prepend_size_ = prepend_.length();

  ParseColumnSpec(spec);
}

HierarchicalList::~HierarchicalList() {
  for (int i = 0; i < columns_.size(); ++i) {
    delwin(columns_[i]);
  }
  delwin(win_);
}

void HierarchicalList::SetDatasource(HierarchicalListDataSource* d) {
  datasource_ = d;
  UpdateFlattenedItems();
}

void HierarchicalList::Draw() {
  // Clear all of the columns
  for (int i = 0; i < columns_.size(); ++i) {
    wclear(columns_[i]);
  }

  int lines_used = -top_line_;
  for (int i = 0; i < NumRoots(); ++i) {
    lines_used += Draw(Root(i), lines_used, 0);
  }

  if (draw_column_headers_) {
    int col_start = 1;
    wattron(win_, COLOR_PAIR(1) | A_BOLD);
    int col_height = 1;
    if (!name_.empty()) col_height += 2;
    for (int i = 0; i < columns_.size(); ++i) {
      int col_width = CursesUtils::winwidth(columns_[i]);
      int xstart = col_start + (int)floor(col_width / 2) -
                   (int)ceil(column_names_[i].length() / 2);
      mvwprintw(win_, col_height, xstart, "%s", column_names_[i].c_str());

      col_start += CursesUtils::winwidth(columns_[i]) + 1;
    }
    wattroff(win_, COLOR_PAIR(1) | A_BOLD);

    // Draw an underline
    mvwhline(win_, col_height + 1, 1, ACS_HLINE,
             CursesUtils::winwidth(win_) - 2);
  }

  // Now that we're done drawing text, draw the frills:
  window_info info = CursesUtils::get_window_info(win_);
  int width = info.width;
  int height = info.height;

  // Draw the border around the frill window
  box(win_, 0, 0);

  // Draw the scrollbar
  const int sbstart = (name_.empty() ? 1 : 3) + (draw_column_headers_ ? 2 : 0);
  for (int i = sbstart; i < height - 1; ++i) {
    mvwaddch(win_, i, width - 2, ACS_CKBOARD);
  }

  // Draw the block.
  if (total_lines_) {
    mvwaddch(
        win_,
        sbstart + lrintf(selected_line_ * (column_height_ - 1) / total_lines_),
        width - 2, ACS_BLOCK);
  }

  // Draw the lines between columns:
  int draw_at = 1;
  for (int c = 0; c < columns_.size(); ++c) {
    draw_at += CursesUtils::winwidth(columns_[c]);
    mvwvline(win_, sbstart - (draw_column_headers_ ? 2 : 0), draw_at, ACS_VLINE,
             column_height_ + (draw_column_headers_ ? 2 : 0));
    ++draw_at;
  }

  if (!name_.empty()) {
    // Draw the title
    CursesUtils::print_in_middle(win_, 1, 0, width, name_.c_str(),
                                 COLOR_PAIR(2));

    // Draw the divider between title and text
    wmove(win_, 2, 0);
    waddch(win_, ACS_LTEE);
    for (int i = 1; i < width - 1; ++i) {
      waddch(win_, ACS_HLINE);
    }
    waddch(win_, ACS_RTEE);
  }

  // Fix things up with some plus symbols:
  int col_start = 1;
  for (int i = 0; i < columns_.size(); ++i) {
    col_start += CursesUtils::winwidth(columns_[i]);
    int xheight = 0;
    if (!name_.empty()) {
      xheight += 2;
    }
    mvwaddch(win_, xheight, col_start, ACS_TTEE);

    if (draw_column_headers_) {
      xheight += 2;
      mvwaddch(win_, xheight, col_start, ACS_PLUS);
    }
    ++col_start;
  }
  //
  // paint column ends
  draw_at = 1;
  for (int c = 0; c < columns_.size(); ++c) {
    draw_at += CursesUtils::winwidth(columns_[c]);
    mvwaddch(win_, height - 1, draw_at, ACS_BTEE);
    ++draw_at;
  }

  mvwaddch(columns_[0], win_rel_selected_line_, 0, '%');

  // Paint us to the virtual screen.  A subsequent call to doupdate() is needed
  // to paint the virtual screen to the real screen.
  wnoutrefresh(win_);
}

int HierarchicalList::Draw(ListItem* node, int line_num, int indent) {
  int max_lines_used = 0;
  for (int c = 0; c < columns_.size(); ++c) {
    WINDOW* column = columns_[c];
    string text = "";
    if (!c) text += (node->ShouldExpand() ? prepend_ : string("+ "));
    text += node->TextForColumn(column_names_[c]);

    wmove(column, line_num, indent);
    int curx = !c ? indent : 0;  // Only indent the first column
    int cury = line_num;

    // If we're selected reverse the text
    if (node == selected_item_) {
      int attrs = A_UNDERLINE | node->ListColor();
      if (c == selected_column_) {
        attrs |= A_REVERSE;
      }
      wattron(column, attrs);
    } else {
      wattron(column, node->ListColor());
    }

    int lines_used = 1;
    int column_width = CursesUtils::winwidth(column);
    for (int i = 0; i < text.size(); ++i) {
      // Figure out how many characters from i to the next white space.
      int chars_to_ws = StrUtils::chars_to_whitespace(text, i);
      if (curx + chars_to_ws > column_width) {
        curx = 0;
        // We only indent the first column
        if (!c) curx += indent + prepend_size_;
        ++cury;
        ++lines_used;
      }
      // https://stackoverflow.com/questions/3911536/utf-8-unicode-whats-with-0xc0-and-0x80
      // https://github.com/MayamaTakeshi/sngrep/commit/869263de0951e3a51bd374dc5b05b70159059a06
      if (isascii(text.c_str()[i])){
            mvwaddch(column, cury, curx++, text.c_str()[i]);
      } else { // deal with utf-8, where more than 1 byte may be needed to represent a character
          int len = 0, j=i;
          while (text.c_str()[i]!='\0') { //count how many chars we have to go through to come up with
              // a printable character
              len += (text.c_str()[i++] & 0xc0) != 0x80;
          }
          mvwaddnstr(column, cury, curx, &text.c_str()[j], i-j);
          curx += len;
      }
    }

    // If we're selected, we're done reversing the text.
    if (node == selected_item_) {
      int attrs = A_UNDERLINE | node->ListColor();
      if (c == selected_column_) {
        attrs |= A_REVERSE;
      }
      wattroff(column, attrs);
    } else {
      wattroff(column, node->ListColor());
    }
    if (lines_used > max_lines_used) {
      max_lines_used = lines_used;
    }
  }

  // now that we've drawn ourselves, draw our children:
  if (node->ShouldExpand()) {
    for (int i = 0; i < node->NumListChildren(); ++i) {
      // We only want to indent the first column's items.
      int ind = indent + indent_;
      max_lines_used +=
          Draw(node->ListChild(i), line_num + max_lines_used, ind);
    }
  }
  return max_lines_used;
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
                 flattened_items_[selected_item_->Index() - 1]->Height() <=
             0) {
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
                 flattened_items_[selected_item_->Index() + 1]->Height() >
             column_height_) {
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
    if (total_lines_ > column_height_) {
      // We need to scroll things to get to the last guy.
      top_line_ = total_lines_ - (column_height_ - 1);
      win_rel_selected_line_ = column_height_ - 1;
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

  if (win_rel_selected_line_ + 1 == column_height_ - 1) {
    // We're going to be at the bottom of the window.
    ++top_line_;
  } else {
    ++win_rel_selected_line_;
  }
  ++selected_line_;
  selected_item_ = item;
}

void HierarchicalList::ScrollToTop() {
  if (flattened_items_.size()) {
    SelectItem(0, SCROLL_TOP);
    SelectNoItem();
  }
}

void HierarchicalList::ScrollToBottom() {
  if (flattened_items_.size()) {
    SelectItem(flattened_items_.size() - 1);
    SelectNoItem();
  }
}

int HierarchicalList::NumLinesDownInList(ListItem* item) {
  int d = 0;
  for (int i = 0; i < flattened_items_.size(); ++i) {
    if (item == flattened_items_[i]) break;
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
    if (total_lines_ >= column_height_) {
      // We have enough items to put one at the bottom.
      selected_item_ = flattened_items_[item_index];
      top_line_ = NumLinesDownInList(selected_item_) +
                  selected_item_->Height() - column_height_;
      selected_line_ = NumLinesDownInList(selected_item_);
      win_rel_selected_line_ = column_height_ - selected_item_->Height();
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

bool HierarchicalList::ParseColumnSpec(const ColumnSpec& spec) {
  if (spec.spec.empty()) {
    return false;
  }

  int top_offset = 1;                         // Account for the top border.
  if (!name_.empty()) top_offset += 2;        // Title and divider line.
  if (draw_column_headers_) top_offset += 2;  // Column name and divider line.
  int left_offset = 1;                        // Account for the left border.
  int usable_width = width_ - 3;  // Left and right borders + scroll bar.
  int usable_height = height_ - (1 + top_offset);  // bottom border + top border

  // First we need to split things on commas:
  vector<string> column_specs;
  StrUtils::SplitStringUsing(",", spec.spec, &column_specs);

  // Now that we know how many columns we have, we need to update the
  // usable_width variable to take into account the lines drawn between columns.
  usable_width -= column_specs.size();

  // Now, compute the widths of any columns that had specified widths:
  vector<int> column_widths;
  int num_exes = 0;
  int used = 0;
  for (int i = 0; i < column_specs.size(); ++i) {
    vector<string> column_info;
    StrUtils::SplitStringUsing(":", column_specs[i], &column_info);
    if (column_info.size() != 2 || column_info[0].empty() ||
        column_info[1].empty()) {
      return false;
    }
    column_names_.push_back(column_info[0]);

    if (column_info[1] == "X" || column_info[1] == "x") {
      column_widths.push_back(-1);
      ++num_exes;
    } else {
      int column_width = atoi(column_info[1].c_str());
      if (!spec.in_percents) {
        column_widths.push_back(column_width);
      } else {
        column_widths.push_back(usable_width *
                                static_cast<int>(column_width / 100.0));
      }
      // Allow room for the title
      column_widths[i] = std::max(column_widths[i],
                                  static_cast<int>(column_names_[i].length()));
      used += column_widths.back();
    }
  }

  // Fill in any unspecified columns.  If there is more than one, they split the
  // remaining free space evenly.
  if (num_exes) {
    int free_col_width =
        static_cast<int>(floor((usable_width - used) / num_exes));
    for (int i = 0; i < column_widths.size(); ++i) {
      if (column_widths[i] == -1) {
        column_widths[i] = free_col_width;
      }
    }
  }

  // Next for each column we have to create the window that will hold its text.
  for (int i = 0; i < column_widths.size(); ++i) {
    // Create the column:
    int width = column_widths[i];
    WINDOW* col = derwin(win_, usable_height, width, top_offset, left_offset);
    columns_.push_back(col);

    // Put a one char space between cols
    left_offset += width + 1;
  }
  usable_width_ = usable_width;
  column_height_ = usable_height;

  return true;
}

void HierarchicalList::EditSelectedItem() {
  if (selected_item_ == NULL) {
    return;
  }

  string answer = DialogBox::RunMultiLine(
      "Please Edit Task", selected_item_->TextForColumn(column_names_[0]),
      CursesUtils::winwidth(win_) / 3, CursesUtils::winheight(win_) / 3);

  if (!answer.empty()) {
    selected_item_->SetListText(answer);
  }
  UpdateFlattenedItems();
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

  // Compute the heights of all of our list items.
  item_for_line_.clear();
  for (int i = 0; i < flattened_items_.size(); ++i) {
    ListItem* item = flattened_items_[i];

    // Figure out how much space we have for each column this list item will
    // draw in.  We want the height of the whole list item to be the max of the
    // heights it takes up in every column.
    int height = 1;
    for (int c = 0; c < columns_.size(); ++c) {
      int testing_width = CursesUtils::winwidth(columns_[c]);
      int prepend_size = 0;
      if (!c) {
        testing_width -= item->Depth() * indent_;
        prepend_size = prepend_.size();
      }
      string text = "";
      if (!c) {
        text += prepend_;
      }
      text += item->TextForColumn(column_names_[c]);
      int height_in_col_c =
          StrUtils::HeightOfTextInWidth(testing_width, text, prepend_size);
      if (height_in_col_c > height) {
        height = height_in_col_c;
      }
    }
    item->SetHeight(height);
    for (int j = 0; j < height; ++j) {
      item_for_line_.push_back(item);
    }
    total_lines_ += item->Height();
  }
}

void HierarchicalList::PreOrderAddToList(ListItem* l) {
  flattened_items_.push_back(l);
  if (l->ListParent() != NULL) {
    l->SetDepth(l->ListParent()->Depth() + 1);
  } else {
    l->SetDepth(0);
  }

  for (int i = 0; i < l->NumListChildren(); ++i) {
    if (l->ShouldExpand()) {
      PreOrderAddToList(l->ListChild(i));
    }
  }
}

ListItem* HierarchicalList::ItemForLineNumber(int n) {
  if (n < 0 || n >= item_for_line_.size()) return NULL;

  return item_for_line_[n];
}
