#pragma once
#ifndef __TILE__
#define __TILE__

#include "Label.h"
#include "NavigationObject.h"
#include "TileStatus.h"
#include "NeighbourTile.h"

class Tile : public NavigationObject
{
public:
	// Constructor
	Tile();

	// Destructor
	~Tile();

	// Life-Cycle Functions
	void Draw() override;
	void Update() override;
	void Clean() override;

	// Getters (Accessors) and Setters (Mutators)
	Tile* GetNeighbourTile(NeighbourTile position) const;
	void SetNeighbourTile(NeighbourTile position, Tile* tile);

	float GetTileCost() const;
	void SetTileCost(float cost);

	TileStatus GetTileStatus() const;
	void SetTileStatus(TileStatus status);

	// Utility Functions
	void AddLabels();
	void SetLabelsEnabled(bool state) const;

private:
	// Private Instance Members (Fields)
	float m_cost;
	TileStatus m_status;

	Label* m_costLabel;
	Label* m_statusLabel;

	Tile* m_neighbours[static_cast<int>(NeighbourTile::NUM_OF_NEIGHBOUR_TILES)];
};

#endif /* defined (__TILE__) */

