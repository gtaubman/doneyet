#ifndef HIERARCHICAL_LIST_H_
#define HIERARCHICAL_LIST_H_

#include <ncurses.h>

#include <string>
#include <vector>

// The column spec consists of one or more <column name>:<column width> pairs.
// The width can also be an X or an x which will give that column an even share
// of any free space left over after all columns with specified width have been
// created.  If in_percents isn't set the column width is assumed to be in
// characters.  If it's set to true, it is assumed to mean a percentage of the
// available window width.  Some examples:
//   "Column One:88,Column Two:12"
// with in_percent set to true would give two columns.
// If we want one big column, one 8 character wide column, and another big
// column, the following will achieve that with in_percent set to false:
//   "Column One:X,Column Two:8,Column Three:X"

using std::string;
using std::vector;

struct ColumnSpec {
  ColumnSpec(const string& s, bool is_percent)
      : spec(s), in_percents(is_percent) {}
  string spec;
  bool in_percents;
};

class ListItem {
 public:
  ListItem();
  virtual ~ListItem();
  virtual const string TextForColumn(const string& c) = 0;
  virtual int NumListChildren() = 0;
  virtual ListItem* ListChild(int i) = 0;
  virtual ListItem* ListParent() = 0;
  virtual void SetListText(const string& text) = 0;
  virtual int ListColor() { return 0; }

  virtual int Height() { return height_; }
  virtual void SetHeight(int h) { height_ = h; }
  virtual int Index() { return index_; }
  virtual void SetIndex(int i) { index_ = i; }
  virtual int Depth() { return depth_; }
  virtual void SetDepth(int d) { depth_ = d; }
  virtual bool ShouldExpand() { return should_expand_; }
  virtual void ToggleExpanded() {
    if (NumListChildren()) should_expand_ = !should_expand_;
  }

 private:
  int height_;          // How many lines this entry takes up.
  int index_;           // Which number item we are in the list.
  int depth_;           // How many parents we have before getting to a root.
  bool should_expand_;  // Whether or not to draw this node's children.
};

// This is a protocol defining how data can be accessed by the HierarchicalList.
// It supports multiple table columns, each with a separate title.
class HierarchicalListDataSource {
 public:
  virtual ~HierarchicalListDataSource() {}
  virtual int NumRoots() = 0;
  virtual ListItem* Root(int i) = 0;
};

enum ScrollType {
  SCROLL_TOP,
  SCROLL_NONE,
  SCROLL_BOTTOM,
};

class HierarchicalList {
 public:
  HierarchicalList(string& name, int height, int width, int y, int x,
                   const ColumnSpec& column_spec);
  virtual ~HierarchicalList();

  void SetDatasource(HierarchicalListDataSource* d);

  void Draw();
  void Update() { UpdateFlattenedItems(); }
  void SelectPrevItem();
  void SelectNextItem();

  void SelectPrevLine();
  void SelectNextLine();

  void ScrollToTop();
  void ScrollToBottom();
  void SelectNoItem() { SelectItem(-1); }
  void SelectNextColumn() { ++selected_column_ %= columns_.size(); }

  void EditSelectedItem();
  ListItem* SelectedItem() { return selected_item_; }

  void SetPrependText(string p) {
    prepend_ = p;
    prepend_size_ = p.length();
  }

  void ToggleExpansionOfSelectedItem();

 private:
  int Draw(ListItem* node, int line_num, int indent);
  void UpdateFlattenedItems();
  void PreOrderAddToList(ListItem* l);
  ListItem* ItemForLineNumber(int n);
  int NumLinesDownInList(ListItem* item);
  void SelectItem(int item_index);
  void SelectItem(int item_index, ScrollType type);
  bool ParseColumnSpec(const ColumnSpec& spec);

  // Convenience methods:
  int NumRoots() { return datasource_->NumRoots(); }
  ListItem* Root(int i) { return datasource_->Root(i); }

  // win_ holds all the frills for the window such as the title bar and the
  // scrollbar and any vertical lines being drawn for column dividers.
  WINDOW* win_;
  int height_;
  int width_;
  int column_height_;
  int usable_width_;

  // This is a list of columns in order.
  vector<WINDOW*> columns_;
  vector<string> column_names_;

  // Which column we currently have selected.
  int selected_column_;

  // Which line to show at the top of the window.  Goes from 0 to the sum of all
  // the heights of all the items.
  int top_line_;

  // The individual line we want selected from 0 to the sum of all heights.
  int selected_line_;

  // The individual line that is selected, goes from 0 to sub_win_.height
  int win_rel_selected_line_;

  // The sum of all heights.
  int total_lines_;

  // How much to indent each level.
  int indent_;

  // If wanted, hierarchical lists can display a small header with a title.
  // This is good for when they're being used as a whole window.
  string name_;

  ListItem* selected_item_;

  // The datasource tells us such things as how many roots we have, how many
  // columns to draw and what data corresponds with which column.
  HierarchicalListDataSource* datasource_;

  // These two vectors are for performance improvements to the drawing code.
  vector<ListItem*> flattened_items_;
  vector<ListItem*> item_for_line_;

  // The following string is prepended to every task.  Default's to "- ".
  string prepend_;
  int prepend_size_;

  // This determines whether or not to align the text in a line after scooting
  // right to display the prepended text.  Example:
  // When true:
  // - Blah blah ...
  //   blah blah.
  // When false:
  // - Blah blah ...
  // blah blah.
  bool flush_left_text_border_;

  bool draw_column_headers_;
};

#endif  // HIERARCHICAL_LIST_H_
