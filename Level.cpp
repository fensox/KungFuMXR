#include "Level.h"
#include "FensoxUtils.h"
#include "StickMan.h"
#include <fstream>
#include <sstream>
#include <tuple>
#include <iostream>

// Constructor takes path to metadata file for the level relative to game executable and an SDLMan pointer to hold for rendering.
// Note load() must be called after construction of this object before other functions will work.
Level::Level(std::string filename, std::weak_ptr<SDLMan> sdlMan) {
	mMetaFile = filename;
    mSDL = sdlMan;
    mColRects = std::make_unique<std::vector<SDL_Rect>>();
}

// Struct to hold sprite info for one sprite for the current level. See level metadata file for member descriptions. The Sprite member of the struct is
// a shared_ptr so it has the option of being shared with the viewport and followed around for cut scenes, etc.
struct Level::SpriteStruct {
    std::shared_ptr<Sprite> sprite{ nullptr };
    decimal spawnX{ 0 };
    decimal spawnY{ 0 };
    decimal triggerX{ 0 };
    bool visible{ false };
    char greatLess{ 'G' };
};

// Destructor
Level::~Level() {
    if constexpr (FuGlobals::DEBUG_MODE) std::cerr << "Destructor: Level" << std::endl;
    for (int i{}; i < mSprites->size(); ++i) mSprites->at(i).sprite.reset();
    mColRects.reset();
    mBGTexture.reset();
    mFollowSprite.reset();
    mSDL.reset();
}



// Loads in the level
bool Level::load() {
    bool success{ true };

    // Initialize/reset all member variables
    resetLevel();

    // Load in the level's metadata file
    if (!loadDataFile()) success = false;

    // Load in the level's music file
    if (!loadMusicFile()) success = false;

    // Load in the level's background texture
    if (!loadBGTexture()) success = false;

    //***DEBUG***
    if constexpr (FuGlobals::DEBUG_MODE) std::cout << toString();

    return success;
}

// Initialize/reset all level variables. Used on game initialization and also to clear old data when loading a new level.
void Level::resetLevel() {
    // initialize our Sprite holding vector
    mSprites = nullptr;
    mSprites = std::make_unique<std::vector<SpriteStruct>>();
}

// Load in the level's background texture. Returns success.
bool Level::loadBGTexture() {
    bool useTrans{ true };
    if (mTrans.a == 0) useTrans = false;
    mBGTexture = mSDL.lock()->loadImage(mBGFile, mTrans, useTrans);
    
    return (!(mBGTexture == nullptr));
}

// Load the level's music file into SDLMan.
bool Level::loadMusicFile() {
    return (mSDL.lock()->loadMusic(mMusicFile));
}

// Load in the level's metadata file
bool Level::loadDataFile() {
    using namespace FensoxUtils;

    bool success{ true };

    // attempt to open a filestream on the filename or return a failure.
    std::ifstream fileStream{ mMetaFile };
    if (!fileStream) return false;

    // parse the file line by line storing values in appropriate member variables. Lines beginning with # ignored as comments.
    std::string strInput{};
    std::string key{}, lastKey{ "#LASTKEY" }; // be sure it is unmatchable the first time (using comment character assures that)
    std::string value{};
    while (std::getline(fileStream, strInput)) {
        // create a string stream from line we read in
        std::istringstream stream(strInput);
        if (std::getline(stream, key, '=')) {
            // trim leading and trailing whitespace, uppercase, and if it is a comment skip
            strTrim(key);
            key = strToUpper(key);
            if (key[0] == '#' || key.empty()) continue;

            // we have the key now get the value and store it in different places depending on what the key is
            std::getline(stream, value);
            switch (hash(key.c_str())) {
                case hash("NAME"):
                    mName = value;
                    break;
                case hash("BACKGROUND"):
                    mBGFile = value;
                    break;
                case hash("MUSIC"):
                    mMusicFile = value;
                    break;
                case hash("PLAYER_START"):
                    mPlayStart = FensoxUtils::getPointFromCDV(value);
                    break;
                case hash("TRANS_COLOR"):
                    if (!storeTrans(value)) success = false;
                    break;
                case hash("COLRECT"):
                    if (!storeColRect(value)) success = false;
                    break;
                case hash("SPRITE"):
                    if (!storeSprite(value)) success = false;
                    break;
                default:
                    std::cerr << "Warning in Level::load. Undefined key value in level metadata file. Key was: " << key;
                    std::cerr << "\nAttempting to continue reading metadata file." << std::endl;
            }
        }
    }

    return success;
}

// Takes a comma delimited string sprite values from the level's metadata file and loads the sprite into a the Level's sprite vector member.
bool Level::storeSprite(std::string value) {
    bool success{ true };

    // parse the string into a vector of strings and return false if it does not contain the right amount of data
    std::vector<std::string> vector{ FensoxUtils::getVectorFromCDV(value, true) };
    if (vector.size() != 5) {
        std::cerr << "Error in Level::storeSprite. Sprite data from metadata file was formatted wrong. Data was: " << value << std::endl;
        return false;
    }

    // format data to correct variable types
    std::string name{ vector[0] };
    decimal spawnX{}, spawnY{}, playerX{};
    try {
        spawnX = std::stoi(vector[1]);
        spawnY = std::stoi(vector[2]);
        playerX = std::stoi(vector[3]);
    } catch (const std::exception& e) {
        std::cerr << "Error in Level::storeSprite. Could not convert string to int some of the Sprite data from metadata file. Error was: " << e.what() << std::endl;
        return false;
    }
    FensoxUtils::strToUpper(vector[4]);
    char greatLess{ vector[4].c_str()[0] };
    
    // get a Sprite object based on the provided name
    std::shared_ptr<Sprite> sprite{ loadSprite(name) };
    if (sprite == nullptr) return false;

    // initialize the sprite
    sprite->load();
    sprite->setLevel( mLevel );
    sprite->setX(spawnX);
    sprite->setY(spawnY);

    // store data in a SpriteStruct
    SpriteStruct ss{ std::move(sprite), spawnX, spawnY, playerX, false, greatLess };
    mSprites->push_back( std::move(ss) );

    if constexpr (FuGlobals::DEBUG_MODE) {
        std::cout << "Level::Sprite data: " << name << ", " << spawnX << ", " << spawnY << ", " << playerX << ", " << greatLess << std::endl;
    }

    return success;
}

// Takes the name of a sprite and returns a pointer to the corresponding Sprite object or nullptr on failure.
std::unique_ptr<Sprite> Level::loadSprite(std::string name) {
    using namespace FensoxUtils;
    
    std::unique_ptr<Sprite> sprite{ nullptr };
    switch (hash(name.c_str())) {
        case hash("STICKMAN"):
            sprite = std::make_unique<StickMan>(mSDL);
            break;
        default:
            std::cerr << "Error in Level::loadSprite. No such sprite exists named: " << name << std::endl;
    }
    
    return std::move(sprite);
}

// Helper function to take a comma delimited value from metadata file, convert to an SDL_Color, and store in our mTrans member. Returns success or failure.
bool Level::storeTrans(std::string value) {
    // Using a helper function, turn comma delimited values into an SDL_Rect
    std::tuple<bool, SDL_Rect> tplRect = FensoxUtils::getRectFromCDV(value);

    // check if we were successful in converting to an SDL_Rect
    if (std::get<0>(tplRect)) {
        SDL_Rect tmpRect = std::get<1>(tplRect);
        mTrans = { static_cast<Uint8>(tmpRect.x),
                   static_cast<Uint8>(tmpRect.y),
                   static_cast<Uint8>(tmpRect.w),
                   static_cast<Uint8>(tmpRect.h)
        };
    } else {
        // helper funct tells us we failed parsing CDVs so output an error msg and return failure
        std::cerr << "Failed in Level::storeTrans. FensoxUtils::getRectfromCDV returned false." << std::endl;
        return false;
    }

    return true;
}

// Helper function to take a comma delimited value, convert to an SDL_Rect, and store in our ColRects member. Returns success or failure.
bool Level::storeColRect(std::string value) {
    // Using a helper function, turn comma delimited values into an SDL_Rect
    std::tuple<bool, SDL_Rect> tplRect = FensoxUtils::getRectFromCDV(value);

    // check if we were successful in converting to an SDL_Rect
    if (std::get<0>(tplRect)) {
        mColRects->push_back(std::get<1>(tplRect));
    } else {
        // helper funct tells us we failed parsing CDVs so output an error msg and return failure
        std::cerr << "Failed in Level::storeColRect. FensoxUtils::getRectfromCDV returned false." << std::endl;
        return false;
    }

    return true;
}

// Set's the player object so the level can query player information.
void Level::setPlayer(std::weak_ptr<Sprite> player) {
    mPlayer = player;

    // loop through all level Sprite's and set their targeting sprite to the new player.
    for (int i{}; i < mSprites->size(); ++i) {
        mSprites->at(i).sprite->setTargetSprite(mPlayer);
    }
}

// Return player start position
SDL_Point Level::getPlayStart() {
    return mPlayStart;
}

// Returns the viewport's top-left coordinates within the level.
SDL_Point Level::getPosition() {
    return mViewport;
}

// Returns the level information represented as a string
std::string Level::toString() {
    std::ostringstream str{};
    str << "Level::Name: " << mName << "\n";
    str << "Level::Metafile: " << mMetaFile << "\n";
    str << "Level::Background: " << mBGFile << "\n";
    str << "Level::Music: " << mMusicFile << "\n";
    str << "Level::Player Start: " << mPlayStart.x << ", " << mPlayStart.y << "\n";
    str << "Level::# Collision Rectangles: " << mColRects->size() << "\n";
    str << "Level::Collision Rectangles List: ";
    for (int i{ 0 }; i < mColRects->size(); ++i) {
        str << "\nLevel::" << mColRects->at(i).x << ", " << mColRects->at(i).y << ", " << mColRects->at(i).w << ", " << mColRects->at(i).h;
    }
    str << std::endl;
    return str.str();
}

// Returns the height and width of the level background in an SDL_Point.
SDL_Point Level::getSize() {
    return mSDL.lock()->getSize( *(mBGTexture.get()) );
}

// Set's the Sprite pointer that this level's viewport will stay centered on.
void Level::setFollowSprite(std::weak_ptr<Sprite> follow) {
    mFollowSprite = follow;
}

// Set's the Sprite that this level's viewport will stay centered on. Parameter set's which sprite to follow as an index value of the level's vector of Sprite objects.
// An overloaded version of this function exists to follow a sprite by pointer.
void Level::setFollowSprite(int follow) {
    mFollowSprite = mSprites->at(follow).sprite;
}

// Set's the current Level being played.
void Level::setLevel(std::weak_ptr<Level> level) {
    mLevel = level;
}

// Centers the viewport on given x, y coordinates adjusting for level boundries and movement buffer specified in FuGlobals::VIEWPORT_BUFFER.
void Level::centerViewport() {
    // get center coordinates of viewport and sprite position
    int centerX{ FuGlobals::VIEWPORT_WIDTH / 2 };
    int centerY{ FuGlobals::VIEWPORT_HEIGHT / 2 };
    int spriteX{ static_cast<int>(mFollowSprite.lock()->getX()) };
    int spriteY{ static_cast<int>(mFollowSprite.lock()->getY()) };

    // calculate travel limits of sprite before we must scroll viewport
    int leftBound{ mViewport.x + centerX - FuGlobals::VIEWPORT_BUFFER / 2 };
    int rightBound{ mViewport.x + centerX + FuGlobals::VIEWPORT_BUFFER / 2 };
    int upBound{ mViewport.y + centerY - FuGlobals::VIEWPORT_BUFFER / 2 };
    int downBound{ mViewport.y + centerY + FuGlobals::VIEWPORT_BUFFER / 2 };

    // move viewport over sprite allowing for buffer zone in middle where we don't scroll. Size of buffer is FuGlobals::VIEWPORT_BUFFER    
    if (spriteX < leftBound) mViewport.x -= leftBound - spriteX;
    else if (spriteX > rightBound) mViewport.x += spriteX - rightBound;
    if (spriteY < upBound) mViewport.y -= upBound - spriteY;
    else if (spriteY > downBound) mViewport.y += spriteY - downBound;

    // check against level boundries and move back some if need be
    int limitX{ mBGTexture->getSize().x - FuGlobals::VIEWPORT_WIDTH };
    int limitY{ mBGTexture->getSize().y - FuGlobals::VIEWPORT_HEIGHT };
    // x
    if (mViewport.x > limitX) mViewport.x = limitX;
    else if (mViewport.x < 0) mViewport.x = 0;
    // y
    if (mViewport.y > limitY) mViewport.y = limitY;
    else if (mViewport.y < 0) mViewport.y = 0;
}

// Outlines all the collision rectangles in the level so visible on screen. Debugging and level design utility function.
void Level::drawColRects() {
    // Set drawing color and loop through all collision rectangles drawing them
    mSDL.lock()->setDrawColor(0, 255, 0);
    for (int i{ 0 }; i < mColRects->size(); ++i) {
        SDL_Rect r = mColRects->at(i);   
        r.x -= mViewport.x; // compensate for viewport's distance from level origin
        r.y -= mViewport.y;
        SDL_RenderDrawRect(mSDL.lock()->getRenderer(), &r);
    }

    // Return our draw color to black
    mSDL.lock()->setDrawColor(0, 0, 0, 255);
}

// Checks if the given point is contained in a collision rect for the level.
// Sprite parameter is to be sure sprite's are not checking for collisions with themselves.
bool Level::isACollisionPoint(const SDL_Point& pnt) {
    // loop through all our level collision rectangles checking for a collision
    for (int i{}; i < mColRects->size(); ++i) {
        const SDL_Rect& r = mColRects->at(i);
        if (SDL_PointInRect(&pnt, &r)) return true;
    }

    return false;
}

// Checks if the given point is contained in a collision rect for the level. Parameter of PointF is cast to integer type SDL_Point.
// Sprite parameter is to be sure sprite's are not checking for collisions with themselves.
bool Level::isACollisionPoint(PointF pnt) {
    return isACollisionPoint(pnt.getSDL_Point());
}

// Checks if the given rectangle is intersecting a collision rectangle for the level.
// Sprite parameter is to be sure sprite's are not checking for collisions with themselves.
bool Level::isACollisionRect(const SDL_Rect& rect) {
    // loop through all our level collision rectangles checking for a collision
    for (int i{}; i < mColRects->size(); ++i) {
        const SDL_Rect& r = mColRects->at(i);
        if (SDL_HasIntersection(&rect, &r)) return true;
    }

    return false;
}

// Checks if the given line is intersecting a collision rectangle for the level.
// Sprite parameter is to be sure sprite's are not checking for collisions with themselves.
bool Level::isACollisionLine(Line line) {
    // loop through all our level collision rectangles checking for a collision
    for (int i{ 0 }; i < mColRects->size(); ++i) {
        const SDL_Rect& r = mColRects->at(i);
        if (SDL_IntersectRectAndLine(&r, &line.x1, &line.y1, &line.x2, &line.y2)) return true;
    }

    return false;
}

// Checks if the given line is colliding with another sprite.
// Sprite parameter is to be sure sprite's are not checking for collisions with themselves.
bool Level::isACollisionSprite(Line line, const Sprite& sprite) {
    // loop through all our level sprite's checking for a collision
    for (int i{}; i < mSprites->size(); ++i) {
        SpriteStruct& ss = mSprites->at(i);
        if (ss.sprite.get() == &sprite) continue;

        SDL_Rect r = ss.sprite->getCollisionRect();
        if (SDL_IntersectRectAndLine(&r, &line.x1, &line.y1, &line.x2, &line.y2)) return true;
    }

    return false;
}

// Processes all non-player sprite movement per frame
void Level::moveSprites() {
    for (std::size_t i{}; i < mSprites->size(); ++i) {
        SpriteStruct& ss = mSprites->at(i);

        if (isSpawnTime(ss)) ss.visible = true;

        if (ss.visible) ss.sprite->move();
    }
}

// Render all non-player sprites to drawing buffer
void Level::renderSprites() {
    for (std::size_t i{}; i < mSprites->size(); ++i) {
        SpriteStruct ss{ mSprites->at(i) };
        
        if (ss.visible) ss.sprite->render();
    }
}

// Accepts a SpriteStruct and calculates if the player has reached the point where sprite should spawn. Returns the result.
bool Level::isSpawnTime(SpriteStruct ss) {
    decimal triggerX{ ss.triggerX };
    decimal playerX{ mPlayer.lock()->getX() };
    char gL{ ss.greatLess };

    switch (gL) {
        case 'G':
            if (playerX > triggerX) return true;
            break;

        case 'L':
            if (playerX < triggerX) return true;
            break;
    }

    return false;
}

// Render the level to the screen
void Level::render() {
    // center viewport on the Sprite we've been told to follow
    centerViewport();

    // put all viewport data into a rectangle for rendering
    SDL_Rect vp{ mViewport.x, mViewport.y, FuGlobals::VIEWPORT_WIDTH, FuGlobals::VIEWPORT_HEIGHT };

    // Draw the area of the level our viewport is pointing at
    SDL_RenderCopyEx(mSDL.lock()->getRenderer(),
        mBGTexture->getTexture(),
        &vp,
        NULL,
        0,
        NULL,
        SDL_FLIP_NONE);

    // Draw all non-player sprites
    renderSprites();

    //***DEBUG*** if debug on draw all collision rectangles so visible on screen
    if constexpr (FuGlobals::DEBUG_MODE) drawColRects();
}