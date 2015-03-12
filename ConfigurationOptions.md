#summary Information about the configurable options.
#labels Featured


# Introduction #

DoneYet supports several runtime configuration options.  They are changeable via a text file named "config" placed in the user's .todo directory.  Configuration sections are preceded by `[SECTION_NAME]`.

## Colors ##
All color options have the following potential values:
  * black
  * red
  * green
  * yellow
  * blue
  * magenta
  * cyan
  * white
  * terminal - This is the terminal's default color.

# Configuration Sections #
## General Options ##
The following options can be found under the `GENERAL` section.
| **Option** | **Default** | **Description** |
|:-----------|:------------|:----------------|
| `foreground_color` | white | The color used for standard drawing. |
| `background_color` | black | The background color. |
| `header_text_color`| red | The color for column headers and text input box headers. |

## Task Options ##
The following options can be found under the `TASKS` section.
| **Option** | **Default** | **Description** |
|:-----------|:------------|:----------------|
| `unstarted_color` | white | The color of an unstarted task. |
| `in_progress_color` | green | The color of a task in progress. |
| `paused_color` | red | The color of a paused task. |
| `finished_color` | blue | The color of a completed task. |
| `prompt_on_delete` | true | Whether or not to prompt for confirmation when deleting a task. |

## Menu Options ##
The following options can be found under the `MENUS` section.
| **Option** | **Default** | **Description** |
|:-----------|:------------|:----------------|
| `bar_foreground_color` | black | The text color for the menu bar.|
| `bar_background_color` | yellow | The background menubar color. |
| `unselected_item_foreground_color` | yellow | The text color for unselected menu items. |
| `unselected_item_background_color` | blue | The background color for unselected menu items. |
| `selected_item_foreground_color` | yellow | The text color for selected menu items. |
| `selected_item_background_color` | blue | The background color for selected menu items. |

# Example Configuration #
The following configuration file would set the foreground color to green, and the background color to whatever color the terminal is currently set to.
```
[GENERAL]
foreground_color = green
background_color = terminal
```