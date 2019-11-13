#include <allegro.h>
#include <string.h>
#include "../headers/player.h"
#include "../headers/vector.h"

Player *player_ref;

volatile int timer_invulnerability = 0;

void increment_invulnerability()
{
    timer_invulnerability++;
    player_ref->invulnerability = 1;

    if (timer_invulnerability > 3)
    {
        player_ref->invulnerability = 0;
    }
}

void init_timer_invulnerability()
{
    timer_invulnerability = 3;
    install_int_ex(increment_invulnerability, SECS_TO_TIMER(1));
}
END_OF_FUNCTION(increment_invulnerability);

LOCK_FUNCTION(increment_invulnerability);

void set_velocity_axis(Player *player, char *axis, float s)
{
    if (!player_ref->taking_damage)
    {
        if (strcmp(axis, "horizontal") == 0)
        {
            player->rb.velocity.x = s;
        }
        else if (strcmp(axis, "vertical") == 0)
        {
            player->rb.velocity.y = s;
        }
    }
}

void onCollisionEnter(RigidBody *self, RigidBody *other)
{
    if (strcmp(other->cb.tag, "ground") == 0)
    {
        if (self->cb.max.y < other->cb.min.y || self->cb.min.y > other->cb.max.y)
        {
            if (self->cb.max.y < other->cb.min.y)
            {
                player_ref->can_jump = 1;
            }
            self->velocity.y = 0;
            player_ref->taking_damage = 0;
            self->acceleration = create_vector(0, 0);
        }
    }
    if ((strcmp(other->cb.tag, "bat") == 0 || strcmp(other->cb.tag, "fox") == 0) && player_ref->invulnerability == 0)
    {
        player_ref->life--;
        player_ref->taking_damage = 1;

        if (other->pos.x > self->pos.x)
        {
            player_ref->rb.velocity.x = -10;
            player_ref->rb.velocity.y = -5;
        }
        else
        {
            player_ref->rb.velocity.x = 10;
            player_ref->rb.velocity.y = -5;
        }

        if (player_ref->taking_damage == 1)
        {
            timer_invulnerability = 0;
            increment_invulnerability();
        }
    }
}

void onCollisionStay(RigidBody *self, RigidBody *other)
{
    if (strcmp(other->cb.tag, "ground") == 0)
    {
        if (self->cb.max.y < other->cb.min.y || self->cb.min.y > other->cb.max.y)
        {
            self->velocity.y = 0;
            self->acceleration = create_vector(0, 0);
            player_ref->can_jump = 1;
            player_ref->taking_damage = 0;
        }
    }
}

void onCollisionExit(RigidBody *self, RigidBody *other)
{
    if (strcmp(other->cb.tag, "ground") == 0)
    {
        player_ref->can_jump = 0;
    }
}

void init_player(Player *player, Vector pos)
{
    player->animation_frame = 8;
    player->facing_right = 1;
    player->taking_damage = 0;
    player->attacking = 0;
    player->life = 100;
    player->invulnerability = 0;

    player->rb.acceleration = create_vector(0, 0);
    player->rb.gravity_scale = 0.1f;
    player->rb.pos = pos;
    player->rb.velocity = create_vector(0, 0);

    player->rb.cb.width = 23;
    player->rb.cb.height = 67;
    player->rb.cb.offset = create_vector(53, 37);
    player->rb.cb.min = create_vector(player->rb.pos.x + player->rb.cb.offset.x, player->rb.pos.y + player->rb.cb.offset.y);
    player->rb.cb.max = create_vector(player->rb.cb.min.x + player->rb.cb.width, player->rb.cb.min.y + player->rb.cb.height);
    player->rb.cb.solid = 1;
    strcpy(player->rb.cb.tag, "player");

    player->rb.onCollisionEnter = onCollisionEnter;
    player->rb.onCollisionExit = onCollisionExit;
    player->rb.onCollisionStay = onCollisionStay;
    player->rb.collidingWith = createList();
    player->can_jump = 0;

    player_ref = player;
}