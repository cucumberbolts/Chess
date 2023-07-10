#pragma once

#include <cstdint>

struct FramebufferSpecification {
	uint32_t Width, Height;

	bool SwapChainTarget = false;  // Render to screen
};

class Framebuffer {
public:
	Framebuffer(const FramebufferSpecification& spec);
	~Framebuffer();

	void Bind() const;
	void Unbind() const;

	const FramebufferSpecification& GetSpecification() const { return m_Specification; }

	uint32_t GetColourAttachment() const { return m_ColourAttachment; }

	void Resize(uint32_t width, uint32_t height);

	void Invalidate();  // Recreate framebuffer from specification
private:
	FramebufferSpecification m_Specification;

	uint32_t m_RendererID = 0;
	uint32_t m_ColourAttachment = 0;
	uint32_t m_DepthAttachment = 0;
};
