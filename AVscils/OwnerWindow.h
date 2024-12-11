#pragma once
#include <Windows.h>
#include <SFML/Graphics.hpp>
#include "ButtonUI.h"
#include "NewDataAll.h"
#include "ConfigMaps.h"
#include "CoutGuide.h"

class OwnerWindow//6100 > 4200 > 2992
{
public:
	void draw(const bool t_isVisibleLoad);
	void processingButton(const sf::Event::MouseButtonEvent& event, bool isWindow2Visible[]);
	void processingButtonMenu(const sf::Event::MouseButtonEvent& event, bool isWindow2Visible[]);
	void processingGuide();
	const bool getCoutGuideActive() const;
	void initialize();

	void activeGameTrue(const HWND& hWndWindow);
	void activeGameFalse();
	void updateRect(const HWND& hWndWindow);
	void UpdateWinow2();

	void setIsVisibleMenu(const bool t_IsVisibleMenu);
	const bool getIsVisibleMenu() const;

private:
	void initializeButtonsUsersDataCommands();
	void setupWindow();
	int mouseButtonPressed(const sf::Event::MouseButtonEvent& event, bool isWindow2Visible[]);
	int mouseButtonMenuPressed(const sf::Event::MouseButtonEvent& event, bool isWindow2Visible[]);
	void updatePosition(const sf::Vector2f& newPoition, const sf::Vector2f& windowPoition, const sf::Vector2f& windowWidthHeight);

	sf::Texture m_TextureButton;
	ButtonUI m_Buttons;
	std::array<ButtonUI, 5> m_ButtonsUsers;
	ButtonUI m_ButtonsMenu;
	ButtonUI m_CoutUserName;
	ButtonUI m_ShapeTrueVisibleMainMenu;
	sf::RectangleShape m_ShapeFalseVisibleMainMenu;
	sf::Sprite m_SpriteIsLoad;

	CoutGuide m_CoutGuide;

	const int m_ConstSize{ 20 };
};

