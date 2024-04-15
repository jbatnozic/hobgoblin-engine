// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/RmlUi/Private/RmlUi_Hobgoblin_renderer.hpp>

#include <RmlUi/Core.h>

#include <GL/glew.h>

#include <Hobgoblin/Private/Pmacro_define.hpp>
HOBGOBLIN_NAMESPACE_BEGIN
namespace rml {
namespace detail {

void RmlUiHobgoblinRenderer::setRenderTarget(gr::RenderTarget* aRenderTarget) {
    _renderTarget = aRenderTarget;
}

gr::RenderTarget* RmlUiHobgoblinRenderer::getRenderTarget() const {
    return _renderTarget;
}

///////////////////////////////////////////////////////////////////////////
// INHERITED FROM RML::RENDERINTERFACE                                   //
///////////////////////////////////////////////////////////////////////////

void RmlUiHobgoblinRenderer::RenderGeometry(
    Rml::Vertex* aVertices,
    int aVerticesCount,
    int* aIndices,
    int aIndicesCount,
    Rml::TextureHandle aTexture,
    const Rml::Vector2f& aTranslation
) {
#define USE_NEW_IMPLEMENTATION
#if defined(USE_NEW_IMPLEMENTATION)
    if (!_renderTarget) {
        return;
    }
    _renderTarget->pushGLStates();
    _initViewport();

    glTranslatef(aTranslation.x, aTranslation.y, 0);

    glVertexPointer(2, GL_FLOAT, sizeof(Rml::Vertex), &aVertices[0].position);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Rml::Vertex), &aVertices[0].colour);

    if (auto sfTexture = reinterpret_cast<sf::Texture*>(aTexture)) {
        glEnable(GL_TEXTURE_2D);

        sf::Texture::bind(sfTexture);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Rml::Vertex), &aVertices[0].tex_coord);
    }
    else {
        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    glDrawElements(GL_TRIANGLES, aIndicesCount, GL_UNSIGNED_INT, aIndices);

    _renderTarget->popGLStates();
#else
    if (!_renderTarget) {
        return;
    }
    (*_renderTarget)->pushGLStates();
    _initViewport();

    glTranslatef(aTranslation.x, aTranslation.y, 0);

    Rml::Vector<Rml::Vector2f> positions(aVerticesCount);
    Rml::Vector<Rml::Colourb> colors(aVerticesCount);
    Rml::Vector<Rml::Vector2f> texCoords(aVerticesCount);

    for (int i = 0; i < aVerticesCount; i++)
    {
        positions[i] = aVertices[i].position;
        colors[i] = aVertices[i].colour;
        texCoords[i] = aVertices[i].tex_coord;
    };

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, &positions[0]);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, &colors[0]);
    glTexCoordPointer(2, GL_FLOAT, 0, &texCoords[0]);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (auto sfTexture = reinterpret_cast<sf::Texture*>(aTexture)) {
        sf::Texture::bind(sfTexture);
    }
    else {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindTexture(GL_TEXTURE_2D, 0);
    };

    glDrawElements(GL_TRIANGLES, aIndicesCount, GL_UNSIGNED_INT, aIndices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glColor4f(1.f, 1.f, 1.f, 1.f);

    (*_renderTarget)->popGLStates();
#endif

}

Rml::CompiledGeometryHandle RmlUiHobgoblinRenderer::CompileGeometry(Rml::Vertex*, int, int*, int, const Rml::TextureHandle) {
    return reinterpret_cast<Rml::CompiledGeometryHandle>(nullptr);
}

void RmlUiHobgoblinRenderer::RenderCompiledGeometry(Rml::CompiledGeometryHandle, const Rml::Vector2f&) {
    RMLUI_ASSERT(false);
}

void RmlUiHobgoblinRenderer::ReleaseCompiledGeometry(Rml::CompiledGeometryHandle) {
    RMLUI_ASSERT(false);
}

bool RmlUiHobgoblinRenderer::LoadTexture(Rml::TextureHandle& texture_handle,
                                         Rml::Vector2i& texture_dimensions,
                                         const Rml::String& source) {
    Rml::FileInterface* file_interface = Rml::GetFileInterface();
    Rml::FileHandle file_handle = file_interface->Open(source);
    if (!file_handle)
        return false;

    file_interface->Seek(file_handle, 0, SEEK_END);
    size_t buffer_size = file_interface->Tell(file_handle);
    file_interface->Seek(file_handle, 0, SEEK_SET);

    char* buffer = new char[buffer_size];
    file_interface->Read(buffer, buffer_size, file_handle);
    file_interface->Close(file_handle);

    sf::Texture* texture = new sf::Texture();

    if (!texture->loadFromMemory(buffer, buffer_size))
    {
        delete[] buffer;
        delete texture;

        return false;
    };
    delete[] buffer;

    texture_handle = reinterpret_cast<Rml::TextureHandle>(texture);
    texture_dimensions = Rml::Vector2i(texture->getSize().x, texture->getSize().y);

    return true;
}

bool RmlUiHobgoblinRenderer::GenerateTexture(Rml::TextureHandle& texture_handle,
                                             const Rml::byte* source,
                                             const Rml::Vector2i& source_dimensions) {
    sf::Texture* texture = new sf::Texture();

    if (!texture->create(source_dimensions.x, source_dimensions.y)) {
        delete texture;
        return false;
    }

    texture->update(source, source_dimensions.x, source_dimensions.y, 0, 0);
    texture_handle = reinterpret_cast<Rml::TextureHandle>(texture);

    return true;
}

void RmlUiHobgoblinRenderer::ReleaseTexture(Rml::TextureHandle texture_handle) {
    delete (reinterpret_cast<sf::Texture*>(texture_handle));
}

void RmlUiHobgoblinRenderer::EnableScissorRegion(bool aEnable) {
    if (aEnable) {
        glEnable(GL_SCISSOR_TEST);
    }
    else {
        glDisable(GL_SCISSOR_TEST);
    }
}

void RmlUiHobgoblinRenderer::SetScissorRegion(int x, int y, int width, int height) {
    if (!_renderTarget) {
        return;
    }
    glScissor(x, _renderTarget->getSize().y - (y + height), width, height);
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE                                                               //
///////////////////////////////////////////////////////////////////////////

void RmlUiHobgoblinRenderer::_initViewport() {
    if (!_renderTarget) {
        return;
    }

    const auto size = _renderTarget->getSize();

    glViewport(0, 0, size.x, size.y);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, size.x, size.y, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

} // namespace detail
} // namespace rml
HOBGOBLIN_NAMESPACE_END
#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
