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
  virtual void AppendCharacter(char* c) = 0;

  virtual int ComputeHeight(int width);
  virtual int Height() { return height_; }
  virtual void SetHeight(int h) { height_ = h; }
  virtual int Index() { return index_; }
  virtual void SetIndex(int i) { index_ = i; }
  virtual int Depth() { return depth_; }
  virtual void SetDepth(int d) { depth_ = d; }

 private:
  // How many lines this entry takes up.
  int height_;
  int index_;
  int depth_;
};

class HierarchicalList {
 public:
  HierarchicalList(string& name, int height, int width, int y, int x);
  virtual ~HierarchicalList();

  void SetDatasource(vector<ListItem*> roots);

  void Run();
  void Draw();
  void SelectPrevItem();
  void SelectNextItem();

  void SelectPrevLine();
  void SelectNextLine();

  void SelectItem(int item_index);

  void EditSelectedItem();

 private:
  int Draw(ListItem* node, int line_num, int indent);
  void UpdateFlattenedItems();
  void PreOrderAddToList(ListItem* l);
  ListItem* ItemForLineNumber(int n);

  WINDOW* win_;
  WINDOW* subwin_;

  int top_line_;
  int selected_line_;
  int win_rel_selected_line_;
  int total_lines_;
  int indent_;
  string name_;

  ListItem* selected_item_;

  vector<ListItem*> roots_;
  vector<ListItem*> flattened_items_;
  vector<ListItem*> item_for_line_;
};

#endif
