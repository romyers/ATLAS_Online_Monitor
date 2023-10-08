/**
 * @file UIElement.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

using namespace std;

// TODO: We need this to be able to store a button, a plot, a text field,
//       or whatever UI object we care to put on a canvas

// TODO: Store metadata determining where the element is displayed on the 
//       canvas

class UIElement {

public:

	UIElement(TPad *type);
	~UIElement();

	UIElement     (      UIElement &other) = delete;
	bool operator=(const UIElement &other) = delete;

	void draw();

	void destroy();

private:

	void validate() const;

	TPad *type;

};

UIElement::UIElement(TPad *type) : type(type) {}

void UIElement::validate() const {

	if(!type) {

		throw logic_error("UIElement was used after destruction.");

	}

}

void UIElement::draw() {

	validate();

	type->Draw();

}

void UIElement::destroy() {

	validate();

	type->Close();

	delete type;
	type = nullptr;

}

UIElement::~UIElement() {

	if(type) {

		type->Close();

		delete type;
		type = nullptr;

	}

}