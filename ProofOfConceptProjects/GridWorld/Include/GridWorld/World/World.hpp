#pragma once

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics/Render_texture.hpp>
#include <Hobgoblin/Graphics/Sprite_loader.hpp>
#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Utility/Grids.hpp>

#include <GridWorld/Model/Cell.hpp>
#include <GridWorld/Model/Lights.hpp>
#include <GridWorld/Model/Sprites.hpp>

#include <GridWorld/Model/Chunk_state_listener_interface.hpp>
#include <GridWorld/Private/Chunk_storage_handler.hpp>

#include <memory>
#include <optional>
#include <unordered_map>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

class IsometricRenderer;

struct WorldConfiguration {
    hg::PZInteger chunkCountX; //!< Maximum number of chunks along the X axis
    hg::PZInteger chunkCountY; //!< Maximum number of chunks along the Y axix

    hg::PZInteger chunkResolution; //!< Number of cells per chunk along the X and Y axes

    float cellResolution; //!< Width and height of a single cell in the world

    // TODO: max openness -- must not be greater than chunkResolution

    // TODO: std::function to create empty extension
};

// TODO
//    Idea: turn off tile refreshing (refresh all when turning on)
class World : private ChunkStateListenerInterface {
public:
    World(const WorldConfiguration& aConfiguration);

    ~World() override = default;

    // TODO: Cell height (z)

    float getCellResolution() const;

    hg::PZInteger getCellCountX() const;

    hg::PZInteger getCellCountY() const;

    ///////////////////////////////////////////////////////////////////////////
    // LOCKING                                                               //
    ///////////////////////////////////////////////////////////////////////////

    struct EditPermission {

    private:
        EditPermission() = default;
        friend class World;
    };

    std::unique_ptr<EditPermission> getPermissionToEdit();

    ///////////////////////////////////////////////////////////////////////////
    // CELL GETTERS                                                          //
    ///////////////////////////////////////////////////////////////////////////

    // Without locking (no on-demand loading)

    const CellModel* getCellAt(hg::PZInteger aX, hg::PZInteger aY) const;

    const CellModel* getCellAt(hg::math::Vector2pz aPos) const;

    const CellModel* getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) const;

    const CellModel* getCellAtUnchecked(hg::math::Vector2pz aPos) const;

    // With locking (on-demand loading enabled)

    const CellModel& getCellAt(const EditPermission& aPerm, hg::PZInteger aX, hg::PZInteger aY) const;

    const CellModel& getCellAt(const EditPermission& aPerm, hg::math::Vector2pz aPos) const;

    const CellModel& getCellAtUnchecked(const EditPermission& aPerm,
                                        hg::PZInteger         aX,
                                        hg::PZInteger         aY) const;

    const CellModel& getCellAtUnchecked(const EditPermission& aPerm, hg::math::Vector2pz aPos) const;

    ///////////////////////////////////////////////////////////////////////////
    // CELL UPDATERS                                                         //
    ///////////////////////////////////////////////////////////////////////////

    class Editor {
    public:
        // Floor

        void updateFloorAt(hg::PZInteger                          aX,
                           hg::PZInteger                          aY,
                           const std::optional<CellModel::Floor>& aFloorOpt);

        void updateFloorAt(hg::math::Vector2pz aPos, const std::optional<CellModel::Floor>& aFloorOpt);

        void updateFloorAtUnchecked(hg::PZInteger                          aX,
                                    hg::PZInteger                          aY,
                                    const std::optional<CellModel::Floor>& aFloorOpt);

        void updateFloorAtUnchecked(hg::math::Vector2pz                    aPos,
                                    const std::optional<CellModel::Floor>& aFloorOpt);

        // Wall

        void updateWallAt(hg::PZInteger                         aX,
                          hg::PZInteger                         aY,
                          const std::optional<CellModel::Wall>& aWallOpt);

        void updateWallAt(hg::math::Vector2pz aPos, const std::optional<CellModel::Wall>& aWallOpt);

        void updateWallAtUnchecked(hg::PZInteger                         aX,
                                   hg::PZInteger                         aY,
                                   const std::optional<CellModel::Wall>& aWallOpt);

        void updateWallAtUnchecked(hg::math::Vector2pz                   aPos,
                                   const std::optional<CellModel::Wall>& aWallOpt);

    private:
        friend class World;
        Editor(World& aWorld);
        World& _world;
    };

    template <class taCallable>
    void edit(const EditPermission&, taCallable&& aCallable) {
        // TODO: start edit
        Editor editor{*this};
        aCallable(editor);
        // TODO: end edit
    }

    ///////////////////////////////////////////////////////////////////////////
    // CHUNK EXTENSIONS                                                      //
    ///////////////////////////////////////////////////////////////////////////

    // getExtensionAtCell() (mutable/const)
    // getExtensionAtCellUnchecked() (mutable/const)
    // getExtensionAtChunk() (mutable/const)
    // getExtensionsAtChunkUnchecked() (mutable/const)

    ///////////////////////////////////////////////////////////////////////////
    // LIGHTS                                                                //
    ///////////////////////////////////////////////////////////////////////////

    LightId createLight(SpriteId aSpriteId, hg::math::Vector2pz aSize);

    void updateLight(LightId aLightHandle, hg::math::Vector2f aPosition, hg::math::AngleF aAngle);

    void destroyLight(LightId aLightHandle);

    // TODO(createRAIILight())

    // TODO(this shouldn't be exposed like this)
    std::unordered_map<LightId, detail::LightModelExt>::const_iterator lightDataBegin() const {
        return _lights.begin();
    }

    // TODO(this shouldn't be exposed like this)
    std::unordered_map<LightId, detail::LightModelExt>::const_iterator lightDataEnd() const {
        return _lights.end();
    }

private:
    // ===== Config =====

    struct WorldConfigurationExt : WorldConfiguration {
        hg::PZInteger cellCountX;
        hg::PZInteger cellCountY;

        WorldConfigurationExt(const WorldConfiguration& aConfiguration)
            : WorldConfiguration{aConfiguration}
            , cellCountX{chunkCountX * chunkResolution}
            , cellCountY{chunkCountY * chunkResolution} {}
    };

    WorldConfigurationExt _configuration;

    // ===== Cells =====

    // TODO: chunk spooler

    detail::ChunkStorageHandler _chunkStorage;

    // ===== Lights =====

    std::unordered_map<LightId, detail::LightModelExt> _lights;
    LightId                                            _lightIdCounter = 0;

    // ===== Other =====

    bool _generatorMode = false;

    void onChunkLoaded(detail::ChunkId aChunkId, ChunkExtensionInterface* aExtension) override;
    void onChunkUnloaded(detail::ChunkId aChunkId, ChunkExtensionInterface* aExtension) override;

    // ===== Updating cells =====

    void _refreshCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY);

    void _updateFloorAt(hg::PZInteger                          aX,
                        hg::PZInteger                          aY,
                        const std::optional<CellModel::Floor>& aFloorOpt);

    void _updateFloorAt(hg::math::Vector2pz aPos, const std::optional<CellModel::Floor>& aFloorOpt);

    void _updateFloorAtUnchecked(hg::PZInteger                          aX,
                                 hg::PZInteger                          aY,
                                 const std::optional<CellModel::Floor>& aFloorOpt);

    void _updateFloorAtUnchecked(hg::math::Vector2pz                    aPos,
                                 const std::optional<CellModel::Floor>& aFloorOpt);

    void _updateWallAt(hg::PZInteger                         aX,
                       hg::PZInteger                         aY,
                       const std::optional<CellModel::Wall>& aWallOpt);

    void _updateWallAt(hg::math::Vector2pz aPos, const std::optional<CellModel::Wall>& aWallOpt);

    void _updateWallAtUnchecked(hg::PZInteger                         aX,
                                hg::PZInteger                         aY,
                                const std::optional<CellModel::Wall>& aWallOpt);

    void _updateWallAtUnchecked(hg::math::Vector2pz                   aPos,
                                const std::optional<CellModel::Wall>& aWallOpt);
};

///////////////////////////////////////////////////////////////////////////
// CELL UPDATES                                                          //
///////////////////////////////////////////////////////////////////////////

// Floor

inline void World::Editor::updateFloorAt(hg::PZInteger                          aX,
                                         hg::PZInteger                          aY,
                                         const std::optional<CellModel::Floor>& aFloorOpt) {
    _world._updateFloorAt(aX, aY, aFloorOpt);
}

inline void World::Editor::updateFloorAt(hg::math::Vector2pz                    aPos,
                                         const std::optional<CellModel::Floor>& aFloorOpt) {
    _world._updateFloorAt(aPos, aFloorOpt);
}

inline void World::Editor::updateFloorAtUnchecked(hg::PZInteger                          aX,
                                                  hg::PZInteger                          aY,
                                                  const std::optional<CellModel::Floor>& aFloorOpt) {
    _world._updateFloorAtUnchecked(aX, aY, aFloorOpt);
}

inline void World::Editor::updateFloorAtUnchecked(hg::math::Vector2pz                    aPos,
                                                  const std::optional<CellModel::Floor>& aFloorOpt) {
    _world._updateFloorAtUnchecked(aPos, aFloorOpt);
}

// Wall

inline void World::Editor::updateWallAt(hg::PZInteger                         aX,
                                        hg::PZInteger                         aY,
                                        const std::optional<CellModel::Wall>& aWallOpt) {
    _world._updateWallAt(aX, aY, aWallOpt);
}

inline void World::Editor::updateWallAt(hg::math::Vector2pz                   aPos,
                                        const std::optional<CellModel::Wall>& aWallOpt) {
    _world._updateWallAt(aPos, aWallOpt);
}

inline void World::Editor::updateWallAtUnchecked(hg::PZInteger                         aX,
                                                 hg::PZInteger                         aY,
                                                 const std::optional<CellModel::Wall>& aWallOpt) {
    _world._updateWallAtUnchecked(aX, aY, aWallOpt);
}

void World::Editor::updateWallAtUnchecked(hg::math::Vector2pz                   aPos,
                                          const std::optional<CellModel::Wall>& aWallOpt) {
    _world._updateWallAtUnchecked(aPos, aWallOpt);
}

} // namespace gridworld
