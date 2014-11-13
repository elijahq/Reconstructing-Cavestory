#include <cmath>
#include "player.h"
#include "game.h"
#include "animated_sprite.h"
#include "graphics.h"

namespace {
// Walk Motion
const float kSlowdownFactor = 0.8f;
const float kWalkingAcceleration = 0.0012f; // (pixels / ms) / ms
const float kMaxSpeedX = 0.325f; // pixels / ms

// Fall Motion
const float kGravity = 0.0012f; // (pixels / ms) / ms
const float kMaxSpeedY = 0.325f; // pixels / ms

// Jump Motion
const float kJumpSpeed = 0.325f; // pixels / ms
const int kJumpTime = 275; // ms

// Sprites
const std::string kSpriteFilePath("content/MyChar.bmp");

// Sprite Frames
const int kCharacterFrame = 0;

const int kWalkFrame = 0;
const int kStandFrame = 0;
const int kJumpFrame = 1;
const int kFallFrame = 2;
const int kUpFrameOffset = 3;
const int kDownFrame = 6;
const int kBackFrame = 7;

// Walk Animation
const int kNumWalkFrames = 3;
const int kWalkFps = 15;
}

bool operator<(const Player::SpriteState& left,
               const Player::SpriteState& right) {
    if(left.motion_type != right.motion_type)
        return left.motion_type < right.motion_type;

    if (left.horizontal_facing != right.horizontal_facing)
        return left.horizontal_facing < right.horizontal_facing;

    if (left.vertical_facing != right.vertical_facing)
        return left.vertical_facing < right.vertical_facing;

    return false;
}

Player::Player(Graphics& graphics, int x, int y) :
        x_(x),
        y_(y),
        velocity_x_ (0.0f),
        velocity_y_ (0.0f),
        acceleration_x_ (0.0f),
        horizontal_facing_(LEFT),
        vertical_facing_(HORIZONTAL),
        on_ground_(false) {
    initializeSprites(graphics);
}

void Player::update(int elapsed_time_ms) {
    sprites_[getSpriteState()]->update(elapsed_time_ms);
    jump_.update(elapsed_time_ms);

    x_ += round(velocity_x_ * elapsed_time_ms);
    velocity_x_ += acceleration_x_ * elapsed_time_ms;

    if (acceleration_x_ < 0.0f)
        velocity_x_ = std::max(velocity_x_, -kMaxSpeedX);
    else if (acceleration_x_ > 0.0f)
        velocity_x_ = std::min(velocity_x_, kMaxSpeedX);
    else if (on_ground_)
        velocity_x_ *= kSlowdownFactor;

    y_ += round(velocity_y_ * elapsed_time_ms);
    if (!jump_.active())
        velocity_y_ = std::min(velocity_y_ + kGravity * elapsed_time_ms,
                               kMaxSpeedY);

    // TODO: remove this fake ground collision "hack"
    if (y_ >= 320) {
        y_ = 320;
        velocity_y_ = 0.0f;
    }

    on_ground_ = y_ == 320;
    /////////////////////////////////////////////////
}

void Player::draw(Graphics& graphics) {
    sprites_[getSpriteState()]->draw(graphics, x_, y_);
}

void Player::startMovingLeft() {
    acceleration_x_ = -kWalkingAcceleration;
    horizontal_facing_ = LEFT;
}

void Player::startMovingRight() {
    acceleration_x_ = kWalkingAcceleration;
    horizontal_facing_ = RIGHT;
}

void Player::stopMoving() {
    acceleration_x_ = 0.0f;
}

void Player::lookUp() {
    vertical_facing_ = UP;
}

void Player::lookDown() {
    vertical_facing_ = DOWN;
}

void Player::lookHorizontal() {
    vertical_facing_ = HORIZONTAL;
}

void Player::startJump() {
    if (on_ground()) {
        jump_.reset();
        // give an initial up velocity
        velocity_y_ = -kJumpSpeed;
    }
    // else if mid jump
    else if (velocity_y_ < 0.0f)
        jump_.reactivate();
}

void Player::stopJump() {
    jump_.deactivate();
}

void Player::initializeSprite(Graphics& graphics,
                              const SpriteState& sprite_state) {
    int source_y = sprite_state.horizontal_facing == LEFT ?
                   kCharacterFrame * Game::kTileSize :
                   (1 + kCharacterFrame) * Game::kTileSize;

    int source_x;
    switch (sprite_state.motion_type) {
        case WALKING:
            source_x = kWalkFrame  * Game::kTileSize;
            break;
        case STANDING:
            source_x = kStandFrame * Game::kTileSize;
            break;
        case JUMPING:
            source_x = kJumpFrame  * Game::kTileSize;
            break;
        case FALLING:
            source_x = kFallFrame  * Game::kTileSize;
            break;
        case LAST_MOTION_TYPE:
            break;
    }
    source_x = sprite_state.vertical_facing == UP ?
               source_x + kUpFrameOffset * Game::kTileSize :
               source_x;

    if (sprite_state.motion_type == WALKING) {
        sprites_[sprite_state] =
            boost::shared_ptr<Sprite>(new AnimatedSprite(
                graphics,
                kSpriteFilePath,
                source_x, source_y,
                Game::kTileSize, Game::kTileSize,
                kWalkFps, kNumWalkFrames));
    }
    else {
        if (sprite_state.vertical_facing == DOWN)
            source_x = sprite_state.motion_type == STANDING ?
                       kBackFrame * Game::kTileSize :
                       kDownFrame * Game::kTileSize;
        sprites_[sprite_state] =
            boost::shared_ptr<Sprite>(new Sprite(
                graphics,
                kSpriteFilePath,
                source_x, source_y,
                Game::kTileSize, Game::kTileSize));
    }
}

void Player::initializeSprites(Graphics& graphics) {
    for (int int_motion_type = FIRST_MOTION_TYPE;
         int_motion_type < LAST_MOTION_TYPE;
         ++int_motion_type
        ) {
        MotionType motion_type
            = static_cast<MotionType>(int_motion_type);

        for (int int_horizontal_facing = FIRST_HORIZONTAL_FACING;
             int_horizontal_facing < LAST_HORIZONTAL_FACING;
             ++int_horizontal_facing
            ) {
            HorizontalFacing horizontal_facing
                = static_cast<HorizontalFacing>(int_horizontal_facing);

            for (int int_vertical_facing = FIRST_VERTICAL_FACING;
                 int_vertical_facing < LAST_VERTICAL_FACING;
                 ++int_vertical_facing
                ) {
                VerticalFacing vertical_facing
                    = static_cast<VerticalFacing>(int_vertical_facing);

                initializeSprite(graphics, SpriteState(motion_type,
                                                       horizontal_facing,
                                                       vertical_facing));
            }
        }
    }
}

Player::SpriteState Player::getSpriteState() {
    MotionType motion;

    if (on_ground())
        motion = acceleration_x_ == 0.0f ? STANDING : WALKING;
    else
        motion = velocity_y_ < 0.0f ? JUMPING : FALLING;


    return SpriteState(motion, horizontal_facing_, vertical_facing_);
}

void Player::Jump::reset() {
    time_remaining_ms_ = kJumpTime;
    reactivate();
}

void Player::Jump::update(int elapsed_time_ms) {
    if (active_) {
        time_remaining_ms_ -= elapsed_time_ms;
        if (time_remaining_ms_ <= 0)
            active_ = false;
    }
}
