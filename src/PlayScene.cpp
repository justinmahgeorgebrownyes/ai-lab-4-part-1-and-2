#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"

// required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"
#include "Util.h"

PlayScene::PlayScene()
{
	PlayScene::Start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::Draw()
{
	DrawDisplayList();
	
	SDL_SetRenderDrawColor(Renderer::Instance().GetRenderer(), 255, 255, 255, 255);
}

void PlayScene::Update()
{
	UpdateDisplayList();
}

void PlayScene::Clean()
{
	RemoveAllChildren();
}

void PlayScene::HandleEvents()
{
	EventManager::Instance().Update();

	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_ESCAPE))
	{
		Game::Instance().Quit();
	}

	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_1))
	{
		Game::Instance().ChangeSceneState(SceneState::START);
	}

	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_2))
	{
		Game::Instance().ChangeSceneState(SceneState::END);
	}
}

void PlayScene::Start()
{
	// Set GUI Title
	m_guiTitle = "Play Scene";

	m_buildGrid(); // construct a Grid of connected tiles

	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	// Add the Target to the Scene
	m_pTarget = new Target(); // instantiate an object of type Target
	m_pTarget->GetTransform()->position = m_getTile(15, 11)->GetTransform()->position + offset;
	m_pTarget->SetGridPosition(15.0f, 11.0f); // record grid space position
	m_getTile(15, 11)->SetTileStatus(TileStatus::GOAL);
	AddChild(m_pTarget);

	// Add the StarShip to the Scene
	m_pStarShip = new StarShip();
	m_pStarShip->GetTransform()->position = m_getTile(1, 3)->GetTransform()->position + offset;
	m_pStarShip->SetGridPosition(1.0f, 3.0f); // record grid space position
	m_getTile(1, 3)->SetTileStatus(TileStatus::START);
	AddChild(m_pStarShip);


	// Preload Sounds

	SoundManager::Instance().Load("../Assets/Audio/yay.ogg", "yay", SoundType::SOUND_SFX);
	SoundManager::Instance().Load("../Assets/Audio/thunder.ogg", "thunder", SoundType::SOUND_SFX);

	ImGuiWindowFrame::Instance().SetGuiFunction(std::bind(&PlayScene::GUI_Function, this));
}

void PlayScene::m_buildGrid()
{
	const auto tile_size = Config::TILE_SIZE; // alias

	// layout a grid of tiles (20 x 15)
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			Tile* tile = new Tile();
			tile->GetTransform()->position = glm::vec2(col * tile_size, row * tile_size); // world position
			tile->SetGridPosition(col, row); // Record the Grid Position
			tile->SetParent(this);
			tile->AddLabels();
			AddChild(tile);
			tile->SetEnabled(false);
			m_pGrid.push_back(tile);

		}
	}

	// setup the neighbour references for each tile in the grid
	// tiles = nodes in our graph
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			Tile* tile = m_getTile(col, row);

			// TopMost Neighbour
			if(row == 0)
			{
				tile->SetNeighbourTile(NeighbourTile::TOP_TILE, nullptr);
			}
			else
			{
				tile->SetNeighbourTile(NeighbourTile::TOP_TILE, m_getTile(col, row - 1));
			}

			// RightMost Neighbour
			if(col == Config::COL_NUM - 1)
			{
				tile->SetNeighbourTile(NeighbourTile::RIGHT_TILE, nullptr);
			}
			else
			{
				tile->SetNeighbourTile(NeighbourTile::RIGHT_TILE, m_getTile(col + 1, row));
			}

			// BottomMost Neighbour
			if (row == Config::ROW_NUM - 1)
			{
				tile->SetNeighbourTile(NeighbourTile::BOTTOM_TILE, nullptr);
			}
			else
			{
				tile->SetNeighbourTile(NeighbourTile::BOTTOM_TILE, m_getTile(col, row + 1));
			}

			// LeftMost Neighbour
			if (col == 0)
			{
				tile->SetNeighbourTile(NeighbourTile::LEFT_TILE, nullptr);
			}
			else
			{
				tile->SetNeighbourTile(NeighbourTile::LEFT_TILE, m_getTile(col - 1, row));
			}
		}
	}
}

bool PlayScene::m_getGridEnabled() const
{
	return m_isGridEnabled;
}

void PlayScene::m_setGridEnabled(const bool state)
{
	m_isGridEnabled = state;
	for (const auto tile : m_pGrid)
	{
		tile->SetEnabled(m_isGridEnabled); // toggles each tile object
		tile->SetLabelsEnabled(m_isGridEnabled); // toggles each label object within the tile
	}
}

void PlayScene::m_computeTileCosts()
{
	// for next lab (4b)
}

Tile* PlayScene::m_getTile(const int col, const int row) const
{
	return m_pGrid[(row * Config::COL_NUM) + col];
}

Tile* PlayScene::m_getTile(const glm::vec2 grid_position) const
{
	const auto col = grid_position.x;
	const auto row = grid_position.y;
	return m_getTile(col, row);
}

void PlayScene::GUI_Function()
{
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();
	
	ImGui::Begin("GAME3001 - W2023 - Lab4", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar );

	ImGui::Separator();

	// Debug Properties
	static bool toggle_grid = false;
	if(ImGui::Checkbox("Toggle Grid", &toggle_grid))
	{
		m_isGridEnabled = toggle_grid;
		m_setGridEnabled(m_isGridEnabled);
	}

	ImGui::Separator();

	// StarShip Properties
	static int start_position[2] = {
		static_cast<int>(m_pStarShip->GetGridPosition().x),
		static_cast<int>(m_pStarShip->GetGridPosition().y)
	};
	if(ImGui::SliderInt2("Start Position", start_position, 0, Config::COL_NUM - 1))
	{
		// constrain the object within max rows
		if(start_position[1] > Config::ROW_NUM - 1)
		{
			start_position[1] = Config::ROW_NUM - 1;
		}

		// convert grid space to world space when snapping the object
		m_getTile(m_pStarShip->GetGridPosition())->SetTileStatus(TileStatus::UNVISITED); // set the tile we left to unvisited
		m_pStarShip->GetTransform()->position =
			m_getTile(start_position[0], start_position[1])->GetTransform()->position + offset;
		m_pStarShip->SetGridPosition(start_position[0], start_position[1]);
		m_getTile(m_pStarShip->GetGridPosition())->SetTileStatus(TileStatus::START);
	}

	ImGui::Separator();
	
	// Target Properties
	static int goal_position[2] = {
		static_cast<int>(m_pTarget->GetGridPosition().x),
		static_cast<int>(m_pTarget->GetGridPosition().y)
	};
	if (ImGui::SliderInt2("Goal Position", goal_position, 0, Config::COL_NUM - 1))
	{
		// constrain the object within max rows
		if (goal_position[1] > Config::ROW_NUM - 1)
		{
			goal_position[1] = Config::ROW_NUM - 1;
		}

		// convert grid space to world space when snapping the object
		m_getTile(m_pTarget->GetGridPosition())->SetTileStatus(TileStatus::UNVISITED); // set the tile we left to unvisited
		m_pTarget->GetTransform()->position =
			m_getTile(goal_position[0], goal_position[1])->GetTransform()->position + offset;
		m_pTarget->SetGridPosition(goal_position[0], goal_position[1]);
		m_getTile(m_pTarget->GetGridPosition())->SetTileStatus(TileStatus::GOAL);
	}

	ImGui::Separator();

	ImGui::End();
}


