#ifndef __HIERARCHICAL_LIST__
#define __HIERARCHICAL_LIST__

#include <ncurses.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

class ListItem {
 public:
  ListItem();
  virtual ~ListItem();
  virtual const string Text() = 0;
  virtual bool ShouldExpand() = 0;
  virtual int NumChildren() = 0;
  virtual ListItem* Child(int i) = 0;
  virtual ListItem* Parent() = 0;
  virtual void SetText(string& text) = 0;

  virtual int ComputeHeight(int width, string& prepend);
  virtual int Height() { return height_; }
  virtual void SetHeight(int h) { height_ = h; }
  virtual int Index() { return index_; }
  virtual void SetIndex(int i) { index_ = i; }
  virtual int Depth() { return depth_; }
  virtual void SetDepth(int d) { depth_ = d; }
  virtual int Color() { return 0; }

 private:
  // How many lines this entry takes up.
  int height_;

  // Which number item we are in the list.
  int index_;

  // How many parents we have before getting to a root.
  int depth_;
};

// This is a protocol defining how data can be accessed by the HierarchicalList.
// It supports multiple table columns, each with a separate title.
class HierarchicalListDataSource {
 public:
  virtual int NumRoots() = 0;
  virtual ListItem* Root(int i) = 0;
  virtual int NumColumns() = 0;
};

enum ScrollType {
  SCROLL_TOP,
  SCROLL_NONE,
  SCROLL_BOTTOM,
};

class HierarchicalList {
 public:
  HierarchicalList(string& name, int height, int width, int y, int x);
  virtual ~HierarchicalList();

  void SetDatasource(HierarchicalListDataSource* d);

  int GetInput();
  void Draw();
  void SelectPrevItem();
  void SelectNextItem();

  void SelectPrevLine();
  void SelectNextLine();

  void SelectNoItem() { SelectItem(-1); }

  void EditSelectedItem();
  ListItem* SelectedItem() { return selected_item_; }

 private:
  int Draw(ListItem* node, int line_num, int indent);
  void UpdateFlattenedItems();
  void PreOrderAddToList(ListItem* l);
  ListItem* ItemForLineNumber(int n);
  int NumLinesDownInList(ListItem* item);
  void SelectItem(int item_index);
  void SelectItem(int item_index, ScrollType type);

  // Convenience methods:
  int NumRoots() { return datasource_->NumRoots(); }
  ListItem* Root(int i) { return datasource_->Root(i); }

  // These two windows hold the the frills such as title and scrollbar, and the
  // actual text.
  WINDOW* win_;
  WINDOW* subwin_;

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

};

#endif
