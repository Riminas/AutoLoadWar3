#pragma once
#include <Windows.h>
#include <SFML/Graphics.hpp>
#include "ButtonUI.h"
#include "NewDataAll.h"
#include "ConfigMaps.h"

class OwnerWindow//6100 > 4200 > 2992 > 3800
{
public:
	void draw(const bool isVisibleLoad);
	void processingButton(const sf::Event::MouseButtonEvent& event, std::array<bool, 2>& isWindow2Visible);
	void processingButtonMenu(const sf::Event::MouseButtonEvent& event, std::array<bool, 2>& isWindow2Visible);
	void initialize();

	void activeGameTrue(const HWND& hWndWindow);
	void activeGameFalse();
	void updateRect(const HWND& hWndWindow);

	void setIsVisibleMenu(const bool t_IsVisibleMenu);
	const bool getIsVisibleMenu() const;

private:
	void initializeButtonsUsersDataCommands();
	void setupWindow();
	int mouseButtonPressed(const sf::Event::MouseButtonEvent& event, std::array<bool, 2>& isWindow2Visible);
	int mouseButtonMenuPressed(const sf::Event::MouseButtonEvent& event, std::array<bool, 2>& isWindow2Visible);
	void updatePosition(const sf::Vector2f& newPoition, const sf::Vector2f& windowPoition, const sf::Vector2f& windowWidthHeight);

	sf::Texture m_TextureButton;
	ButtonUI m_Buttons;
	std::array<ButtonUI, 5> m_ButtonsUsers;
	ButtonUI m_ButtonsMenu;
	ButtonUI m_ButtonsMenu2;
	//ButtonUI m_CoutUserName;
	//ButtonUI m_CoutIrinaBot;
	//ButtonUI m_ShapeTrueVisibleMainMenu;
	sf::RectangleShape m_ShapeFalseVisibleMainMenu;
	sf::Sprite m_SpriteIsLoad;

	const int m_ConstSize{ 20 };
};

