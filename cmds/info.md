
# Dialogue System Colors

The following colors can be used in `.line` files with `@Color: "Name"` or `@D-Color: "Name"` tags:

*   Red
*   Green
*   Blue
*   Yellow
*   Cyan
*   Magenta
*   White (Default for text)
*   Black
*   Orange
*   Purple
*   Pink
*   Gray
*   Light Blue
*   Gold (Default for name)

Example Usage:
```
@Name: "Hero"
@Color: "Gold"
@D-Color: "White"
@Dialogue: {
    @Line[1]: "Hello world!"
}
```

# Debug Commands

## `skip`
**Usage**: Type `skip` in the game console.
**Effect**: Immediately kills the boss and triggers the post-boss phase (dialogue).

> [!WARNING]
> This command is for **debugging only**. Using it does not count as a legitimate win because you did not actually kill the boss in combat. It skips the fight mechanics and score awards associated with the kill.
>
> **For the Nerds**: Technically, this command directly manipulates the `bossDead` and `postBossPhase` flags and calls `enemies.clear()`, completely bypassing the `DamageBoss()` event loop. This means no damage verification, score accumulation, or hit validation logic is ever executed. You are entering the post-fight state without satisfying the combat resolution conditions.