#pragma once

#include <array>

#include "Graphics/Application.h"

#include "Graphics/Framebuffer.h"
#include "Graphics/SubTexture.h"

class ChessApplication : public Application {
public:
	ChessApplication(uint32_t width, uint32_t height, const std::string& name);
	ChessApplication(const ChessApplication&) = delete;
	ChessApplication(ChessApplication&&) noexcept = delete;
	~ChessApplication() override = default;

	ChessApplication& operator=(const ChessApplication&) = delete;
	ChessApplication& operator=(ChessApplication&&) = delete;

	void OnInit() override;
	void OnRender() override;

	void RenderImGui() override;
protected:
	void OnWindowClose() override;
	void OnWindowResize(int32_t width, int32_t height) override;
	void OnKeyPressed(int32_t key, int32_t scancode, int32_t action, int32_t mods) override;
	void OnMouseButton(int32_t button, int32_t action, int32_t mods) override;
private:
	Board m_Board;
	Square m_SelectedPiece = INVALID_SQUARE;
	bool m_IsHoldingPiece = false;  // If the selected piece follows the mouse
	BitBoard m_LegalMoves = 0;
	std::string m_BoardFEN;
	
	glm::vec4 m_LightSquareColour = glm::vec4(0);
	glm::vec4 m_DarkSquareColour = glm::vec4(0);
	glm::vec4 m_LegalMoveColour = glm::vec4(0);

	std::shared_ptr<Framebuffer> m_ChessViewport;
	glm::vec2 m_ChessViewportSize;

	std::array<std::shared_ptr<SubTexture>, 12> m_ChessPieceSprites;
};
