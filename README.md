# mod-contributors

AzerothCore module that grants rested XP bonuses to contributor accounts.

## Features

- **Rested XP on login and level-up** for accounts flagged as contributors
- **Timed or permanent bonuses** — grant for X days or use `-1` for permanent
- **Contributor levels** — Bronze, Silver, Gold, Platinum (perks per level coming soon)
- **Player opt-out** — contributors can disable their bonus with `.contributor disable on`
- **Configurable** — enable/disable module, set default duration, set max affected level

## Installation

1. Place this module in `modules/mod-contributors/`
2. Re-run CMake and rebuild
3. Import `data/sql/db-characters/mod_contributors_accounts.sql` into your characters database
4. Copy `conf/mod_contributors.conf.dist` to your server config directory and rename to `mod_contributors.conf`
5. Enable in config: `ModContributors.Enable = 1`

## Configuration

| Setting | Default | Description |
|---------|---------|-------------|
| `ModContributors.Enable` | `0` | Enable/disable the module |
| `ModContributors.Duration` | `30` | Default bonus duration in days |
| `ModContributors.MaxAffectedLevel` | `69` | No bonuses granted at or above this level |

## Commands

### Player commands

| Command | Description |
|---------|-------------|
| `.contributor info` | View your own contributor status |
| `.contributor disable on/off` | Opt out of or back into the rested XP bonus |

### Admin commands (SEC_ADMINISTRATOR)

| Command | Description |
|---------|-------------|
| `.contributor grant <account> [days] [level]` | Grant contributor status. Omit days for config default, use `-1` for permanent. Level: 1-4 (default 1) |
| `.contributor setlevel <account> <level>` | Change contributor level without affecting duration |
| `.contributor revoke <account>` | Remove contributor status |
| `.contributor info <account>` | View any account's contributor status |

### Contributor Levels

| Level | Name |
|-------|------|
| 1 | Bronze |
| 2 | Silver |
| 3 | Gold |
| 4 | Platinum |

## Database

Uses one table in the **characters** database:

```sql
mod_contributors_accounts (
    AccountId    INT UNSIGNED PRIMARY KEY,
    GrantedDate  INT UNSIGNED,  -- when status was granted
    EndDate      INT UNSIGNED,  -- expiry timestamp (0 for permanent)
    Level        TINYINT UNSIGNED,  -- contributor level (1-4)
    Permanent    TINYINT,       -- 1 = never expires
    Expired      TINYINT        -- 1 = timed bonus ran out
)
```

## Credits

- Built for [ChromieCraft](https://www.chromiecraft.com/)
- Based on [AzerothCore](https://www.azerothcore.org/) module framework
- Inspired by [mod-resurrection-scroll](https://github.com/azerothcore/mod-resurrection-scroll)
