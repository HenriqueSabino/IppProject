#include <stdlib.h>
#include "../headers/rigidbody.h"
#include "../headers/constants.h"
#include "../headers/collisionbox.h"
#include "../headers/list.h"

void update(RigidBody *rb)
{
    rb->acceleration.y = rb->gravity_scale * GRAVITY;
    rb->velocity = sum(rb->velocity, rb->acceleration);
    rb->pos = sum(rb->pos, rb->velocity);

    rb->cb.min = sum(rb->pos, rb->cb.offset);
    rb->cb.max.x = rb->cb.min.x + rb->cb.width;
    rb->cb.max.y = rb->cb.min.y + rb->cb.height;
}

void update_all(RigidBody *rbs[], int amount)
{
    CollisionBox prev_cb[amount];
    for (int i = 0; i < amount; i++)
    {
        prev_cb[i] = rbs[i]->cb;
        update(rbs[i]);
    }
    for (int i = 0; i < amount; i++)
    {
        for (int j = 0; j < amount; j++)
        {
            if (i == j)
            {
                continue;
            }

            DataNode other_cb;
            other_cb.value = &rbs[j]->cb;

            if (collided(rbs[i]->cb, rbs[j]->cb) && rbs[i]->cb.solid)
            {

                if (prev_cb[i].min.x >= rbs[j]->cb.max.x)
                {
                    rbs[i]->pos.x = (prev_cb[i].min.x) - (prev_cb[i].min.x - rbs[j]->cb.max.x) - prev_cb[i].offset.x;
                }
                else if (prev_cb[i].max.x <= rbs[j]->cb.min.x)
                {

                    rbs[i]->pos.x = prev_cb[i].min.x + rbs[j]->cb.min.x - prev_cb[i].max.x - prev_cb[i].offset.x;
                }
                else if (prev_cb[i].min.y >= rbs[j]->cb.max.y)
                {
                    rbs[i]->pos.y = prev_cb[i].min.y - (prev_cb[i].min.y - rbs[j]->cb.max.y) - prev_cb[i].offset.y;
                }
                else if (prev_cb[i].max.y <= rbs[j]->cb.min.y)
                {
                    rbs[i]->pos.y = prev_cb[i].min.y + rbs[j]->cb.min.y - prev_cb[i].max.y - prev_cb[i].offset.y;
                }

                rbs[i]->cb.min = sum(rbs[i]->pos, rbs[i]->cb.offset);
                rbs[i]->cb.max = sum(rbs[i]->cb.min, create_vector(rbs[i]->cb.width, rbs[i]->cb.height));
            }
            else
            {
                int index = indexof(rbs[i]->collidingWith, other_cb);
                if (index != -1)
                {
                    if (rbs[i]->onCollisionExit != NULL)
                    {
                        remove_at(rbs[i]->collidingWith, index);
                        (rbs[i]->onCollisionExit)(rbs[i], rbs[j]);
                    }
                }
            }
        }
    }
    for (int i = 0; i < amount; i++)
    {
        for (int j = 0; j < amount; j++)
        {
            if (i == j)
            {
                continue;
            }
            DataNode other_cb;
            other_cb.value = &rbs[j]->cb;

            if (collided(rbs[i]->cb, rbs[j]->cb) && rbs[i]->cb.solid)
            {
                if (indexof(rbs[i]->collidingWith, other_cb) != -1)
                {
                    if (rbs[i]->onCollisionStay != NULL)
                    {
                        (rbs[i]->onCollisionStay)(rbs[i], rbs[j]);
                    }
                }
                else
                {
                    if (rbs[i]->onCollisionEnter != NULL)
                    {
                        push(rbs[i]->collidingWith, other_cb);
                        (rbs[i]->onCollisionEnter)(rbs[i], rbs[j]);
                    }
                }
            }
        }
    }
}

void update_limit_pos(RigidBody *rb, Vector min, Vector max)
{
    update(rb);
    if (rb->pos.x <= min.x)
    {
        rb->pos.x = min.x;
        rb->velocity.x = 0;
    }
    else if (rb->pos.x >= max.x)
    {
        rb->pos.x = max.x;
        rb->velocity.x = 0;
    }
    if (rb->pos.y <= min.y)
    {
        rb->pos.y = min.y;
        rb->velocity.y = 0;
    }
    else if (rb->pos.y >= max.y)
    {
        rb->pos.y = max.y;
        rb->velocity.y = 0;
    }
}

void apply_force(RigidBody *rb, float x, float y)
{
    //force = mass * acceleration
    //mass = 1, for now, so force = acceleration
    rb->acceleration.x += x;
    rb->acceleration.y += y;
}