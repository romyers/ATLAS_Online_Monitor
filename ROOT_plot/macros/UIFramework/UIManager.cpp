/**
 * @file UIManager.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <algorithm>
#include <exception>
#include <vector>

#include "macros/UIFramework/Panel.cpp"

using namespace std;

// TODO: Consider switching to an std::set for panel storage
class UIManager {

public:

    UIManager();

    UIManager     (      UIManager &other) = delete;
    void operator=(const UIManager &other) = delete;

    void refresh();

    bool has(const Panel &panel) const;

    void attach(Panel &panel);
    void detach(const Panel &panel);

private:

    vector<Panel*> panels;

};

UIManager::UIManager() {};

bool UIManager::has(const Panel &panel) const {

    auto iter = find(panels.cbegin(), panels.cend(), &panel);

    return iter != panels.cend();

}

void UIManager::attach(Panel &panel) {

    if(has(panel)) {

        throw logic_error("UIManager::attach -- Panel is already attached!");

    }

    panels.push_back(&panel);

}

// NOTE: This implementation requires that each panel in the UIManager is unique
void UIManager::detach(const Panel &panel) {

    auto iter = find(panels.cbegin(), panels.cend(), &panel);
    if(iter == panels.cend()) {

        throw logic_error("UIManager::detach -- Cannot detach an unattached panel!");

    }

    panels.erase(iter);

}

void UIManager::refresh() {

    for(Panel *panel : panels) {

        panel->update();

    }

    gSystem->ProcessEvents();

}