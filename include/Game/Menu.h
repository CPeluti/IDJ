#pragma once
#include "Core/Component.h"
#include <map>
#include "Core/Vec2.h"
#include "Core/GameObject.h"
#include "Core/InputManager.h"
#include "Core/SpriteRenderer.h"
#include "Core/Animator.h"
#include "Core/Text.h"
#include "Core/Game.h"
#include "Core/Camera.h"
enum MenuStates {
	INITIAL,
	SETTINGS,
	ALL
};

enum Alignement {
	START,
	CENTER,
	END
};


bool y_sort(std::shared_ptr<GameObject> i, std::shared_ptr<GameObject> j);

bool z_sort(std::shared_ptr<GameObject> i, std::shared_ptr<GameObject> j);

class Menu {
public:
	Menu(MenuStates initialState = INITIAL) : currentState(initialState) {
	}
	virtual ~Menu() = default;
	inline void Start() {
		this->elements = std::map < std::string, std::shared_ptr<GameObject>>();
		std::shared_ptr<Animation> buttonAnimation = std::make_shared<Animation>(0, 0, 0.0f);
		std::shared_ptr<Animation> buttonAnimationHover = std::make_shared<Animation>(1, 1, 0.0f);
		std::shared_ptr<Animation> buttonAnimationClick = std::make_shared<Animation>(2, 2, 0.0f);
		
		//Util
		std::shared_ptr<GameObject> uiCursor = std::make_shared<GameObject>();
		std::shared_ptr<GameObject> tabCursor = std::make_shared<GameObject>();
		std::shared_ptr<GameObject> titleBg = std::make_shared<GameObject>();

		std::shared_ptr<SpriteRenderer> titleBgRenderer = std::make_shared<SpriteRenderer>(*titleBg, "resources/interface/ui_pause_scroll.png");
		std::shared_ptr<SpriteRenderer> uiCursorRenderer = std::make_shared<SpriteRenderer>(*uiCursor, "resources/interface/ui_cursor.png");
		std::shared_ptr<SpriteRenderer> tabCursorRenderer = std::make_shared<SpriteRenderer>(*tabCursor, "resources/interface/ui_settings_tabs.png");
		titleBgRenderer->SetCameraFollower(true);
		uiCursorRenderer->SetCameraFollower(true);
		tabCursorRenderer->SetCameraFollower(true);

		elementsStateMap["uiCursor"] = ALL;
		elementsStateMap["tabCursor"] = ALL;
		elementsStateMap["titleBg"] = ALL;
		elements["uiCursor"] = uiCursor;
		elements["tabCursor"] = tabCursor;
		elements["titleBg"] = titleBg;

		// Initial
		std::shared_ptr<GameObject> pauseBg = std::make_shared<GameObject>();
		std::shared_ptr<GameObject> continueButton = std::make_shared<GameObject>();
		std::shared_ptr<GameObject> settingsButton = std::make_shared<GameObject>();
		std::shared_ptr<GameObject> quitButton = std::make_shared<GameObject>();

		std::shared_ptr<SpriteRenderer> pauseBgRenderer = std::make_shared<SpriteRenderer>(*pauseBg, "resources/interface/ui_pause_background.png");
		std::shared_ptr<SpriteRenderer> continueButtonRenderer = std::make_shared<SpriteRenderer>(*continueButton, "resources/interface/ui_button.png", 3, 1);
		std::shared_ptr<SpriteRenderer> settingsButtonRenderer = std::make_shared<SpriteRenderer>(*settingsButton, "resources/interface/ui_button.png", 3, 1);
		std::shared_ptr<SpriteRenderer> quitButtonRenderer = std::make_shared<SpriteRenderer>(*quitButton, "resources/interface/ui_button.png", 3, 1);
		pauseBgRenderer->SetCameraFollower(true);
		continueButtonRenderer->SetCameraFollower(true);
		settingsButtonRenderer->SetCameraFollower(true);
		quitButtonRenderer->SetCameraFollower(true);

		std::shared_ptr<Animator> continueButtonAnimator = std::make_shared<Animator>(*continueButton);
		std::shared_ptr<Animator> settingsButtonAnimator = std::make_shared<Animator>(*settingsButton);
		std::shared_ptr<Animator> quitButtonAnimator = std::make_shared<Animator>(*quitButton);

		pauseBg->AddComponent(pauseBgRenderer);
		continueButton->AddComponent(continueButtonRenderer);
		settingsButton->AddComponent(settingsButtonRenderer);
		quitButton->AddComponent(quitButtonRenderer);
		continueButton->AddComponent(continueButtonAnimator);
		settingsButton->AddComponent(settingsButtonAnimator);
		quitButton->AddComponent(quitButtonAnimator);


		continueButtonAnimator->AddAnimation("default", buttonAnimation.get());
		continueButtonAnimator->AddAnimation("hover", buttonAnimationHover.get());
		continueButtonAnimator->AddAnimation("click", buttonAnimationClick.get());
		settingsButtonAnimator->AddAnimation("default", buttonAnimation.get());
		settingsButtonAnimator->AddAnimation("hover", buttonAnimationHover.get());
		settingsButtonAnimator->AddAnimation("click", buttonAnimationClick.get());
		quitButtonAnimator->AddAnimation("default", buttonAnimation.get());
		quitButtonAnimator->AddAnimation("hover", buttonAnimationHover.get());
		quitButtonAnimator->AddAnimation("click", buttonAnimationClick.get());

		continueButtonAnimator->SetAnimation("default");
		settingsButtonAnimator->SetAnimation("default");
		quitButtonAnimator->SetAnimation("default");
		

		pauseBg->box.Move(Game::GetInstance().GetWindowSize() / 2 / Camera::zoom);
		continueButton->box.RawMove(pauseBg->box.GetPos());
		continueButton->box.MoveToPos({ 16,16  });
		settingsButton->box.RawMove(continueButton->box.GetPos() + Vec2{0.0,continueButton->box.GetSize().y});
		settingsButton->box.MoveToPos({ 0,6  });
		quitButton->box.RawMove(settingsButton->box.GetPos() + Vec2{ 0.0,settingsButton->box.GetSize().y });
		quitButton->box.MoveToPos({ 0,6  });

		pauseBg->z=0;
		continueButton->z=1;
		settingsButton->z=1;
		quitButton->z = 1;
		elements["pauseBg"] = pauseBg;
		elements["continue:button"] = continueButton;
		elements["settings:button"] = settingsButton;
		elements["quit:button"] = quitButton;
		elementsStateMap["pauseBg"] = INITIAL;
		elementsStateMap["continue:button"] = INITIAL;
		elementsStateMap["settings:button"] = INITIAL;
		elementsStateMap["quit:button"] = INITIAL;
		// Settings
		{
			std::shared_ptr<GameObject> settingsBg = std::make_shared<GameObject>();
			std::shared_ptr<GameObject> plusResolutionButton = std::make_shared<GameObject>();
			std::shared_ptr<GameObject> minusResolutionButton = std::make_shared<GameObject>();
			std::shared_ptr<GameObject> plusMusicButton = std::make_shared<GameObject>();
			std::shared_ptr<GameObject> minusMusicButton = std::make_shared<GameObject>();
			std::shared_ptr<GameObject> plusEffectsButton = std::make_shared<GameObject>();
			std::shared_ptr<GameObject> minusEffectsButton = std::make_shared<GameObject>();
			std::shared_ptr<GameObject> backButton = std::make_shared<GameObject>();
			elements["settingsBg"] = settingsBg;
			elements["plusResolution:button"] = plusResolutionButton;
			elements["minusResolution:button"] = minusResolutionButton;
			elements["plusMusic:button"] = plusMusicButton;
			elements["minusMusic:button"] = minusMusicButton;
			elements["plusEffects:button"] = plusEffectsButton;
			elements["minusEffects:button"] = minusEffectsButton;
			elements["back:button"] = backButton;

			elementsStateMap["settingsBg"] = SETTINGS;
			elementsStateMap["plusResolution:button"] = SETTINGS;
			elementsStateMap["minusResolution:button"] = SETTINGS;
			elementsStateMap["plusMusic:button"] = SETTINGS;
			elementsStateMap["minusResolution:button"] = SETTINGS;
			elementsStateMap["plusEffects:button"] = SETTINGS;
			elementsStateMap["minusEffects:button"] = SETTINGS;
			elementsStateMap["back:button"] = SETTINGS;


			std::shared_ptr<SpriteRenderer> settingsBgRenderer = std::make_shared<SpriteRenderer>(*settingsBg, "resources/interface/ui_settings_background.png");
			std::shared_ptr<SpriteRenderer> plusResolutionButtonRenderer = std::make_shared<SpriteRenderer>(*plusResolutionButton, "resources/interface/ui_plus_button.png", 3, 1);
			std::shared_ptr<SpriteRenderer> minusResolutionButtonRenderer = std::make_shared<SpriteRenderer>(*minusResolutionButton, "resources/interface/ui_minus_button.png", 3, 1);
			std::shared_ptr<SpriteRenderer> plusMusicButtonRenderer = std::make_shared<SpriteRenderer>(*plusMusicButton, "resources/interface/ui_plus_button.png", 3, 1);
			std::shared_ptr<SpriteRenderer> minusMusicButtonRenderer = std::make_shared<SpriteRenderer>(*minusMusicButton, "resources/interface/ui_minus_button.png", 3, 1);
			std::shared_ptr<SpriteRenderer> plusEffectsButtonRenderer = std::make_shared<SpriteRenderer>(*plusEffectsButton, "resources/interface/ui_plus_button.png", 3, 1);
			std::shared_ptr<SpriteRenderer> minusEffectsButtonRenderer = std::make_shared<SpriteRenderer>(*minusEffectsButton, "resources/interface/ui_minus_button.png", 3, 1);
			std::shared_ptr<SpriteRenderer> backButtonRenderer = std::make_shared<SpriteRenderer>(*backButton, "resources/interface/ui_button.png", 3, 1);
			plusResolutionButtonRenderer->SetCameraFollower(true);
			settingsBgRenderer->SetCameraFollower(true);
			minusResolutionButtonRenderer->SetCameraFollower(true);
			plusMusicButtonRenderer->SetCameraFollower(true);
			minusMusicButtonRenderer->SetCameraFollower(true);
			plusEffectsButtonRenderer->SetCameraFollower(true);
			minusEffectsButtonRenderer->SetCameraFollower(true);
			backButtonRenderer->SetCameraFollower(true);

			std::shared_ptr<Animator> plusResolutionButtonAnimator = std::make_shared<Animator>(*plusResolutionButton);
			std::shared_ptr<Animator> minusResolutionButtonAnimator = std::make_shared<Animator>(*minusResolutionButton);
			std::shared_ptr<Animator> plusMusicButtonAnimator = std::make_shared<Animator>(*plusMusicButton);
			std::shared_ptr<Animator> minusMusicButtonAnimator = std::make_shared<Animator>(*minusMusicButton);
			std::shared_ptr<Animator> plusEffectsButtonAnimator = std::make_shared<Animator>(*plusEffectsButton);
			std::shared_ptr<Animator> minusEffectsButtonAnimator = std::make_shared<Animator>(*minusEffectsButton);
			std::shared_ptr<Animator> backButtonAnimator = std::make_shared<Animator>(*backButton);

			plusResolutionButtonAnimator->AddAnimation("default", buttonAnimation.get());
			plusResolutionButtonAnimator->AddAnimation("hover", buttonAnimationHover.get());
			plusResolutionButtonAnimator->AddAnimation("click", buttonAnimationClick.get());
			minusResolutionButtonAnimator->AddAnimation("default", buttonAnimation.get());
			minusResolutionButtonAnimator->AddAnimation("hover", buttonAnimationHover.get());
			minusResolutionButtonAnimator->AddAnimation("click", buttonAnimationClick.get());
			plusMusicButtonAnimator->AddAnimation("default", buttonAnimation.get());
			plusMusicButtonAnimator->AddAnimation("hover", buttonAnimationHover.get());
			plusMusicButtonAnimator->AddAnimation("click", buttonAnimationClick.get());
			minusMusicButtonAnimator->AddAnimation("default", buttonAnimation.get());
			minusMusicButtonAnimator->AddAnimation("hover", buttonAnimationHover.get());
			minusMusicButtonAnimator->AddAnimation("click", buttonAnimationClick.get());
			plusEffectsButtonAnimator->AddAnimation("default", buttonAnimation.get());
			plusEffectsButtonAnimator->AddAnimation("hover", buttonAnimationHover.get());
			plusEffectsButtonAnimator->AddAnimation("click", buttonAnimationClick.get());
			minusEffectsButtonAnimator->AddAnimation("default", buttonAnimation.get());
			minusEffectsButtonAnimator->AddAnimation("hover", buttonAnimationHover.get());
			minusEffectsButtonAnimator->AddAnimation("click", buttonAnimationClick.get());
			backButtonAnimator->AddAnimation("default", buttonAnimation.get());
			backButtonAnimator->AddAnimation("hover", buttonAnimationHover.get());
			backButtonAnimator->AddAnimation("click", buttonAnimationClick.get());
		}

		//initial texts
		{
			std::shared_ptr<GameObject> pause = std::make_shared<GameObject>();
			std::shared_ptr<Text> pauseText = std::make_shared<Text>(*pause, "resources/font/neodgm.ttf", 30, Text::SOLID, "Pausa", SDL_Color{ 255, 255, 255 }, 0, true);
			pause->AddComponent(pauseText);
			elements["pause:text"] = pause;

			std::shared_ptr<GameObject> continuar = std::make_shared<GameObject>();
			std::shared_ptr<Text> continuarText = std::make_shared<Text>(*continuar, "resources/font/neodgm.ttf", 30, Text::SOLID, "Continuar", SDL_Color{ 255, 255, 255 }, 0, true);
			continuar->AddComponent(continuarText);
			elements["continuar:text"] = continuar;

			std::shared_ptr<GameObject> settings = std::make_shared<GameObject>();
			std::shared_ptr<Text> settingsText = std::make_shared<Text>(*settings, "resources/font/neodgm.ttf", 30, Text::SOLID, "Opcoes", SDL_Color{ 255, 255, 255 }, 0, true);
			settings->AddComponent(settingsText);
			elements["settings:text"] = settings;

			std::shared_ptr<GameObject> quit = std::make_shared<GameObject>();
			std::shared_ptr<Text> quitText = std::make_shared<Text>(*quit, "resources/font/neodgm.ttf", 30, Text::SOLID, "Sair", SDL_Color{ 255, 255, 255 }, 0, true);
			quit->AddComponent(quitText);
			elements["quit:text"] = quit;

			//pause->box.Move();
			continuar->box.Move(continueButton->box.center());
			settings->box.Move(settingsButton->box.center());
			quit->box.Move(quitButton->box.center());
			continuar->z = 2;
			settings->z = 2;
			quit->z = 2;
			elementsStateMap["pause:text"] = INITIAL;
			elementsStateMap["continuar:text"] = INITIAL;
			elementsStateMap["settings:text"] = INITIAL;
			elementsStateMap["quit:text"] = INITIAL;
		}

		//Setting texts
		{
			std::shared_ptr<GameObject> settings = std::make_shared<GameObject>();
			std::shared_ptr<Text> settingsText = std::make_shared<Text>(*settings, "resources/font/neodgm.ttf", 30, Text::SOLID, "Opções", SDL_Color{ 255, 255, 255 }, 0);
			settings->AddComponent(settingsText);
			elements["settings:text"] = settings;

			std::shared_ptr<GameObject> resolution = std::make_shared<GameObject>();
			std::shared_ptr<Text> resolutionText = std::make_shared<Text>(*resolution, "resources/font/neodgm.ttf", 30, Text::SOLID, "Resolução", SDL_Color{ 255, 255, 255 }, 0);
			resolution->AddComponent(resolutionText);
			elements["resolution:text"] = resolution;

			std::shared_ptr<GameObject> music = std::make_shared<GameObject>();
			std::shared_ptr<Text> musicText = std::make_shared<Text>(*music, "resources/font/neodgm.ttf", 30, Text::SOLID, "Volume Música", SDL_Color{ 255, 255, 255 }, 0);
			music->AddComponent(musicText);
			elements["music:text"] = music;

			std::shared_ptr<GameObject> effects = std::make_shared<GameObject>();
			std::shared_ptr<Text> effectsText = std::make_shared<Text>(*effects, "resources/font/neodgm.ttf", 30, Text::SOLID, "Volume Efeitos", SDL_Color{ 255, 255, 255 }, 0);
			effects->AddComponent(effectsText);
			elements["effects:text"] = effects;

			std::shared_ptr<GameObject> back = std::make_shared<GameObject>();
			std::shared_ptr<Text> backText = std::make_shared<Text>(*back, "resources/font/neodgm.ttf", 30, Text::SOLID, "Voltar", SDL_Color{ 255, 255, 255 }, 0);
			back->AddComponent(backText);
			elements["back:text"] = back;

			std::shared_ptr<GameObject> currentResolutionGO = std::make_shared<GameObject>();
			std::shared_ptr<Text> currentResolutionText = std::make_shared<Text>(*currentResolutionGO, "resources/font/neodgm.ttf", 30, Text::SOLID, fmt::format("{}p", currentResolution.second), SDL_Color{ 255, 255, 255 }, 0);
			currentResolutionGO->AddComponent(currentResolutionText);
			elements["currentResolution:text"] = currentResolutionGO;

			std::shared_ptr<GameObject> currentMusicVolumeGO = std::make_shared<GameObject>();
			std::shared_ptr<Text> currentMusicVolumeText = std::make_shared<Text>(*currentMusicVolumeGO, "resources/font/neodgm.ttf", 30, Text::SOLID, fmt::format("{}", currentMusicVolume), SDL_Color{ 255, 255, 255 }, 0);
			currentMusicVolumeGO->AddComponent(currentMusicVolumeText);
			elements["currentMusicVolumeGO:text"] = currentMusicVolumeGO;

			std::shared_ptr<GameObject> currentEffectVolumeGO = std::make_shared<GameObject>();
			std::shared_ptr<Text> currentEffectVolumeText = std::make_shared<Text>(*currentEffectVolumeGO, "resources/font/neodgm.ttf", 30, Text::SOLID, fmt::format("{}",currentEffectVolume), SDL_Color{255, 255, 255}, 0);
			currentEffectVolumeGO->AddComponent(currentEffectVolumeText);
			elements["currentEffectVolume:text"] = currentEffectVolumeGO;

			elementsStateMap["settings:text"] = SETTINGS;
			elementsStateMap["resolution:text"] = SETTINGS;
			elementsStateMap["music:text"] = SETTINGS;
			elementsStateMap["effects:text"] = SETTINGS;
			elementsStateMap["back:text"] = SETTINGS;
			elementsStateMap["currentResolution:text"] = SETTINGS;
			elementsStateMap["currentMusicVolume:text"] = SETTINGS;
			elementsStateMap["currentEffectVolume:text"] = SETTINGS;

		}


	}
	inline void Update(float dt){
		auto oldState = currentState;
		InputManager& inputManager = InputManager::GetInstance();
		for (auto& element : elements) {
			if ((element.first.find(":button")) != std::string::npos) {
				std::string token = element.first.substr(0, element.first.find(":"));
				clickTimers[token].Update(dt);
				if(clickTimers[token].Expired()) {
					if (auto locked = std::dynamic_pointer_cast<Animator>(element.second->GetComponent("Animator").lock())) {
						if(currentHovered == token) {
							locked->SetAnimation("hover");
							elementsState[token] = "hover";
						} else {
							locked->SetAnimation("default");
							elementsState[token] = "default";
						}
					}
				}
			}
		}
		if (inputManager.KeyPress(LEFT_ARROW_KEY)) { 
			cursorCoordinates.first -= cursorCoordinates.first >= 0 ? 1 : 0; 
		}
		else if (inputManager.KeyPress(RIGHT_ARROW_KEY)){
			cursorCoordinates.first += cursorCoordinates.first < interactOrder[cursorCoordinates.second].size() ? 1 : 0;
		}
		else if(inputManager.KeyPress(UP_ARROW_KEY)){
			cursorCoordinates.second -= cursorCoordinates.second >= 0 ? 1 : 0;
		}
		else if(inputManager.KeyPress(DOWN_ARROW_KEY)){
			cursorCoordinates.second += cursorCoordinates.second < interactOrder.size() ? 1 : 0;
		}
		else if(inputManager.KeyPress(RETURN_KEY)){
			click(cursorCoordinates);
		}
		if(oldState != currentState) {
			for (const auto& element : elements) {
				if (elementsStateMap[element.first] == oldState) {
					if (auto locked = std::dynamic_pointer_cast<Animator>(element.second->GetComponent("Animator").lock())) {
						locked->SetAnimation("default");
						elementsState[element.first] = "default";
					}
				}
			}
		}
	}
	inline void Render() {
		std::vector < std::shared_ptr<GameObject>> renderOrder;
		if (!enabled) return;
		for (const auto& element : elements) {
			if(currentState == elementsStateMap[element.first] || elementsStateMap[element.first] == ALL)
				LOG_INFO("Rendering element: " + element.first);
				renderOrder.push_back(element.second);
		}
		std::stable_sort(renderOrder.begin(), renderOrder.end(), z_sort);
		for (const auto& element : renderOrder) {
			element->Render();
		}
	}
	bool enabled = false;
	inline MenuStates GetCurrentState() const { return currentState; }
	inline void SetCurrentState(MenuStates state) { currentState = state; }
	virtual bool Is(std::string type) { return type == "Menu"; }
	inline std::string click(std::pair<int,int> pos) {
		std::string clickedElement = "";
		for(const auto& element : elements) {
			if ((element.first.find(":button")) != std::string::npos) {
				if (auto locked = std::dynamic_pointer_cast<Animator>(element.second->GetComponent("Animator").lock())) {
					locked->SetAnimation("click");
					std::string token = element.first.substr(0, element.first.find(":"));
					currentClicked = token;
					elementsState[currentClicked] = "click";
					clickTimers[currentClicked].Restart();
				}
			}
		}

		if(clickedElement == "settings")
		{
			currentState = SETTINGS;
				
		}
		else if(clickedElement == "continue")
		{
			currentState = INITIAL;
				
		}
		else if(clickedElement == "exit")
		{
				
		}
		else if(clickedElement == "backSettings")
		{
			currentState = INITIAL;
				
		}
		else if(clickedElement == "decreaseMusicVolume")
		{
			// Decrease volume logic
				
		}
		else if(clickedElement == "increaseMusicVolume")
		{
			// Increase volume logic
				
		}
		else if(clickedElement == "decreaseEffectsVolume")
		{
			// Decrease volume logic
				
		}
		else if(clickedElement == "increaseEffectsVolume")
		{
			// Increase volume logic
				
		}
		else if(clickedElement == "decreaseResolution")
		{
			// Decrease resolution logic
				
		}
		else if(clickedElement == "increaseResolution")
		{
			// Increase resolution logic
				
		}
		return clickedElement;
	};
	//inline std::string hover(Vec2 pos) {
	//	for(const auto& element : elements) {
	//		if (element.second->box.contains(pos)) {
	//			if ((element.first.find(":button")) != std::string::npos) {
	//				if (auto locked = std::dynamic_pointer_cast<Animator>(element.second->GetComponent("Animator").lock())) {
	//					locked->SetAnimation("hover");
	//					std::string token = element.first.substr(0, element.first.find(":"));
	//					if (currentHovered != token) {
	//						if (auto currentHoveredLocked = std::dynamic_pointer_cast<Animator>(elements[currentHovered]->GetComponent("Animator").lock())) {
	//							currentHoveredLocked->SetAnimation("default");
	//							elementsState[currentHovered] = "default";
	//						}
	//					}
	//					currentHovered = token;
	//					elementsState[currentHovered] = "hover";
	//				}
	//			}
	//			return element.first;
	//		}
	//	}
	//	return "";
	//};
protected:

	std::string currentClicked;
	std::string currentHovered;
	std::string currentSelected;
	std::map<std::string, Timer> clickTimers;
	std::pair<int, int> currentResolution = { 1920,1080 };
	int minMusicVolume = 0;
	int minEffectVolume = 0;

	int maxMusicVolume = 10;
	int maxEffectVolume = 10;

	int currentMusicVolume = 5;
	int currentEffectVolume = 5;

	std::pair<int,int> cursorCoordinates = { 0, 0 };
	std::vector<std::vector<std::string>> elementOrder;
	std::vector<std::vector<std::string>> interactOrder;
	MenuStates currentState;

	//ID and element
	std::map < std::string, std::shared_ptr<GameObject>> elements;

	std::map<std::string, MenuStates> elementsStateMap;
	std::map <std::string, std::string> elementsState;

};