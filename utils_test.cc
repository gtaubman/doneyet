#include "utils.h"
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

static int errors = 0;

void ERROR(const string& error) {
    cout << error << endl;;
    ++errors;
}

void TestTrimMultipleSpaces() {
    string single_word = "hi";
    StrUtils::trim_multiple_spaces(single_word);
    if (single_word != "hi") {
        ERROR("Trimming white spaces from a word with no whitespaces is broken.");
    }

    string two_words = "hi gabe";
    StrUtils::trim_multiple_spaces(two_words);
    if (two_words != "hi gabe") {
        ERROR("Trimming spaces when there's only one is broken.");
    }

    string spaced_words = "hi        gabe";
    StrUtils::trim_multiple_spaces(spaced_words);
    if (spaced_words != "hi gabe") {
        ERROR("Trimming multiple space inbetween two words is broken.");
    }

    string end_space = "hi         ";
    StrUtils::trim_multiple_spaces(end_space);
    if (end_space != "hi") {
        ERROR("Trimming spaces at the end of a sentence is broken.");
    }

    string beginning_space = "          hi";
    StrUtils::trim_multiple_spaces(beginning_space);
    if (beginning_space != "hi") {
        ERROR("Trimming spaces at the beginning of a sentence is broken.");
    }
}

void TestSplitStringUsing() {
    vector<string> test;
    StrUtils::SplitStringUsing("-", "hi", &test);
    assert(test.size() == 1);
    if (test[0] != "hi") {
        ERROR("Splitting on non-existent string is broken.");
    }

    test.clear();
    StrUtils::SplitStringUsing("-", "hi-", &test);
    assert(test.size() == 1);
    if (test[0] != "hi") {
        ERROR("Splitting on non-existent string is broken. (Got " + test[0] + ")");
    }

    // Test normal splitting scenario
    test.clear();
    StrUtils::SplitStringUsing("-", "hi-there-gabe", &test);
    assert(test.size() == 3);
    if (test[0] != "hi") {
        ERROR("Test[0] was " + test[0]);
    }
    if (test[1] != "there") {
        ERROR("Test[1] was " + test[1]);
    }
    if (test[2] != "gabe") {
        ERROR("Test[2] was " + test[2]);
    }

    // Test splitting on a splitter with size > 1.
    test.clear();
    StrUtils::SplitStringUsing("--", "hi--there--gabe", &test);
    assert(test.size() == 3);
    if (test[0] != "hi") {
        ERROR("Test[0] was " + test[0]);
    }
    if (test[1] != "there") {
        ERROR("Test[1] was " + test[1]);
    }
    if (test[2] != "gabe") {
        ERROR("Test[2] was " + test[2]);
    }

    // Test splitting when there's a missing spot.
    test.clear();
    StrUtils::SplitStringUsing("-", "hi--there-gabe", &test);
    assert(test.size() == 4);
    if (test[0] != "hi") {
        ERROR("Test[0] was " + test[0]);
    }
    if (!test[1].empty()) {
        ERROR("Test[1] wasn't empty.  It was " + test[1]);
    }
    if (test[2] != "there") {
        ERROR("Test[2] was " + test[2]);
    }
    if (test[3] != "gabe") {
        ERROR("Test[3] was " + test[3]);
    }
}

void TestHeightOfTextInWidth() {
    int width = 107;
    string text = "- Figure out when to select the newly created task and when not to. Or possibly just make this a preference.";
    int height = StrUtils::HeightOfTextInWidth(width, text, 2);
    if (height != 2) {
        ERROR("Computing heights of text is broken.");
    }
}

void TestStrUtils() {
    TestTrimMultipleSpaces();
    TestSplitStringUsing();
    TestHeightOfTextInWidth();
}

int main() {
    TestStrUtils();

    cout << errors << " errors." << endl;
}
