/**
 * @file Panel.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>
#include <exception>
#include <vector>

#include "macros/UIFramework/UIElement.cpp"

using namespace std;

// TODO: Can we prevent panels from being displayed until they're attached to
//       the GUI manager?

// TODO: Panels and UIManagers both share the need to attach elements. Perhaps
//       we should unify these two objects. They also share:
//         -- updating (together with UIElement)
// TODO: In general, Panels, UIManagers, and UIElements would work well as
//       parts of one tree structure

class Panel {

public:

    // TODO: Do we need to provide manual name setting?
    Panel(
        const string &name, 
        const string &title, 
        int left, 
        int top, 
        int width, 
        int height
    );
    ~Panel();

    // Again, we don't really want these copied, since messing with one copy
    // messes with them all, and deep copying will just cause extraneous
    // TCanvases to pop up
    Panel         (      Panel &other) = delete;
    void operator=(const Panel &other) = delete;

    bool has(const UIElement &elt) const;

    void attach(UIElement &elt);
    void detach(UIElement &elt);

    void update  ();
    void destroy ();

private:

    void validate() const;

    TCanvas *canvas;

    vector<UIElement*> elements;

};

Panel::Panel(
    const string &name, 
    const string &title, 
    int left, 
    int top, 
    int width, 
    int height
) {

    canvas = new TCanvas(name.data(), title.data(), left, top, width, height);

}

Panel::~Panel() {

    if(canvas) {

        canvas->Close();

        delete canvas;
        canvas = nullptr;

    }

}

bool Panel::has(const UIElement &elt) const {

    validate();

    auto iter = find(elements.cbegin(), elements.cend(), &elt);

    return iter != elements.cend();

}

void Panel::attach(UIElement &elt) {

    validate();

    if(has(elt)) {

        throw logic_error("panel::attach -- Element is already attached!");

    }

    elements.push_back(&elt);

}

void Panel::detach(UIElement &elt) {

    validate();

    auto iter = find(elements.cbegin(), elements.cend(), &elt);
    if(iter == elements.cend()) {

        throw logic_error("Panel::detach -- Cannot detach an unattached element!");

    }

    elements.erase(iter);



}

void Panel::update() {

    validate();

    canvas->cd();

    for(UIElement *elt : elements) {

        elt->draw();

    }

    canvas->Update();

    // TODO: Implement

}

void Panel::destroy() {

    validate();

    canvas->Close();
    delete canvas;
    canvas = nullptr;

}

void Panel::validate() const {

    if(!canvas) {

        throw logic_error("Panel was used after destruction.");

    }

}