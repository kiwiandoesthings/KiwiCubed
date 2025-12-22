#include "UI.h"
#include "Events.h"
#include "TextRenderer.h"


UI::UI() : uiShaderProgram(uiShaderProgram), uiAtlas(uiAtlas) {}
UI::~UI() = default;

UI& UI::GetInstance() {
    static UI instance;
    return instance;
}

void UI::Setup(Shader shaderProgram, Texture atlas, TextRenderer* textRenderer) {
    OVERRIDE_LOG_NAME("UI Setup");
    if (!renderComponentsSetup) {
        vertexBufferObject.SetupBuffer();
        vertexArrayObject.SetupArrayObject();
        indexBufferObject.SetupBuffer();
    
        renderComponentsSetup = true;
    } else {
        ERR("Tried to setup render components when they were already setup, aborting");
        return;
    }

    uiShaderProgram = shaderProgram;
    uiAtlas = atlas;
    uiTextRenderer = textRenderer;
    window = &Window::GetInstance();

    inputHandler.SetupCallbacks(Window::GetInstance().GetGLFWWindow());
    inputHandler.RegisterMouseButtonCallback(GLFW_MOUSE_BUTTON_LEFT, [&](int button){
        if (currentScreen == nullptr) {
            return;
        }

        for (int iterator = 0; iterator < currentScreen->uiElements.size(); ++iterator) {
            if (currentScreen->uiElements[iterator]->OnClick()) {
                return;
            }
        }
    });
    inputHandler.RegisterKeyCallback(GLFW_KEY_TAB, [&](int key) {
        int totalElements = currentScreen->uiElements.size();
        int tabIndex = currentScreen->GetTabIndex();
        if (tabIndex + 2 > totalElements) {
            currentScreen->SetTabIndex(0);
        } else {
            currentScreen->SetTabIndex(tabIndex + 1);
        }
    });
    inputHandler.RegisterKeyCallback(GLFW_KEY_ENTER, [&](int key) {
        currentScreen->uiElements[currentScreen->GetTabIndex()]->Trigger();
    });

    EventManager& eventManager = EventManager::GetInstance();
    eventManager.RegisterEvent("event/disable_ui");
    eventManager.AddEventToDo("event/disable_ui", [&](Event& event) {
        DisableUI();
    });
    eventManager.RegisterEvent("event/back_ui");
    eventManager.AddEventToDo("event/back_ui", [&](Event& event) {
        if (currentScreen == nullptr) {
            return;
        }
        
        if (!stackedScreens.empty()) {
            stackedScreens.pop();
            if (!stackedScreens.empty()) {
                currentScreen = stackedScreens.top();
            } else {
                currentScreen = nullptr;
                window->SetFocused(true);
            }
        }
    });
}

void UI::Render() {
    if (currentScreen == nullptr) {
        return;
    }

    uiAtlas.SetActive();
    uiAtlas.Bind();
    stackedScreens.top()->Render();
}

void UI::AddScreen(UIScreen screen) {
    uiScreens.push_back(std::make_unique<UIScreen>(std::move(screen)));
}

void UI::SetCurrentScreen(const std::string screenName) {
    UIScreen* uiScreen = GetScreen(screenName);
    stackedScreens.push(uiScreen);
    currentScreen = uiScreen;
}

UIScreen* UI::GetScreen(const std::string& screenName) {
    OVERRIDE_LOG_NAME("UI");
    for (int iterator = 0; iterator < uiScreens.size(); ++iterator) {
        if (uiScreens[iterator]->screenName == screenName) {
            return uiScreens[iterator].get();
        }
    }
    CRITICAL("Tried to get UIScreen with name \"" + screenName + "\" that did not exist");
    psnip_trap();
}

UIScreen* UI::GetCurrentScreen() {
    return currentScreen;
}

std::string UI::GetCurrentScreenName() {
    return currentScreen->screenName;
}

void UI::DisableUI() {
    stackedScreens = std::stack<UIScreen*>{};
    currentScreen = nullptr;

    Window::GetInstance().SetFocused(true);
}

bool UI::IsDisabled() {
    return currentScreen == nullptr;
}

InputHandler& UI::GetInputHandler() {
    return inputHandler;
}

void UI::Delete() {
    OVERRIDE_LOG_NAME("UI");
    INFO("Deleting screens");
    for (int iterator = 0; iterator < uiScreens.size(); ++iterator) {
        uiScreens[iterator].get()->Delete();
    }
    uiScreens.clear();
}

UIScreen::UIScreen(std::string screenName) : screenName(screenName) {
    OVERRIDE_LOG_NAME("UI Initialization");

    std::string realName = screenName.substr(3);

    EventManager::GetInstance().RegisterEvent("ui/move_screen_" + realName);
    EventManager::GetInstance().AddEventToDo("ui/move_screen_" + realName, [=](Event& event) {
        UI::GetInstance().SetCurrentScreen(screenName);
    });

    INFO("Created screen \"" + screenName + "\"");
}

void UIScreen::Render() {
    for (int iterator = 0; iterator < customRenderCommands.size(); iterator++) {
        customRenderCommands[iterator]();
    }
    for (int iterator = 0; iterator < uiElements.size(); iterator++) {
        if (uiElements[iterator]->GetVisible()) {
            uiElements[iterator]->Render();
        }
    }
}

void UIScreen::AddCustomRenderCommand(std::function<void()> customCommand) {
    customRenderCommands.push_back(customCommand);
}

void UIScreen::ClearCustomRenderCommands() {
    customRenderCommands.clear();
}

void UIScreen::AddUIElement(UIElement* element) {
    uiElements.emplace_back(element);
}

int UIScreen::GetTabIndex() {
    return tabIndex;
}

void UIScreen::Delete() {
    OVERRIDE_LOG_NAME("UI Screen");
    INFO("Deleted screen \"" + screenName + "\" with {" + std::to_string(uiElements.size()) + "} elements");
    for (int iterator = 0; iterator < uiElements.size(); iterator++) {
        delete uiElements[iterator];
    }
    uiElements.clear();
}

void UIScreen::SetTabIndex(int newTabIndex) {
    tabIndex = newTabIndex;

    if (tabIndex == 0) {
        uiElements[uiElements.size() - 1]->SetSelected(false);
    } else {
        uiElements[tabIndex - 1]->SetSelected(false);
    }
    uiElements[tabIndex]->SetSelected(true);
}

glm::vec2 UIElement::PixelsToNDC(glm::vec2 pixelPosition) {
    return glm::vec2((pixelPosition.x / Window::GetInstance().GetWidth()) * 2 - 1, (pixelPosition.y / Window::GetInstance().GetHeight()) * 2 - 1);
}

UIElement::UIElement(glm::vec2 position, glm::vec2 scale, std::string eventToTrigger) : position(position), scale(scale), eventToTrigger(eventToTrigger) {
    return;
}

void UIElement::Render() {
    return;
}

void UIElement::Trigger() {
    EventManager::GetInstance().TriggerEvent(eventToTrigger);
}

bool UIElement::OnClick() {
    if (!GetHovered() || eventToTrigger == "") {
        return false;
    }

    EventManager::GetInstance().TriggerEvent(eventToTrigger);
    return true;
}

void UIElement::OnHover() {
    return;
}

std::string* UIElement::GetEventTrigger() {
    return &eventToTrigger;
}

bool UIElement::GetVisible() {
    return visible;
}

void UIElement::SetVisible(bool visible) {
    UIElement::visible = visible;
}

glm::vec2* UIElement::GetPosition() {
    return &position;
}

glm::ivec2* UIElement::GetScale() {
    return &scale;
}

glm::vec2* UIElement::GetSize() {
    return &size;
}

bool UIElement::GetSelected() {
    return tabSelected;
}

void UIElement::SetSelected(bool selected) {
    tabSelected = selected;
}

bool UIElement::GetHovered() {
    glm::vec2 mousePosition = UI::GetInstance().GetInputHandler().GetMousePosition();
    int windowHeight = Window::GetInstance().windowHeight;
    mousePosition.y = windowHeight - mousePosition.y;

    hoverSelected = mousePosition.x >= position.x && mousePosition.y >= position.y && mousePosition.x <= position.x + size.x && mousePosition.y <= position.y + size.y;

    return hoverSelected;
}

UIElement::~UIElement() = default;