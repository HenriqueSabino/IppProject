#ifndef PLAYER_H
#define PLAYER_H

#include "rigidbody.h"

#define PLAYER_SPRITE_SHEET "..\\assets\\PlayerSpriteSheet.bmp"

typedef struct player
{
    RigidBody rb;
    RigidBody sword_rb;
    int animation_frame, facing_right, can_jump, life, taking_damage, attacking, invulnerability,Bow_attack;
} Player;

void increment_invulnerability();
void init_timer_invulnerability();
void init_player(Player *player, Vector pos);
void set_velocity_axis(Player *player, char *axis, float s);


#endif // !"PLAYER_H"