-- Weekly weapon rotation table (admin edits these rows to rotate weapons)
CREATE TABLE IF NOT EXISTS raid_weekly_weapon (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    item_id INT NOT NULL,
    start_date DATETIME NOT NULL,
    end_date DATETIME NOT NULL,
    note VARCHAR(255) NULL
);

-- Per-player progression (persisted)
CREATE TABLE IF NOT EXISTS raid_progression (
    char_id BIGINT PRIMARY KEY,
    progression FLOAT NOT NULL DEFAULT 0,
    last_updated DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

-- Optional run history for audit
CREATE TABLE IF NOT EXISTS raid_run_history (
    run_id BIGINT PRIMARY KEY AUTO_INCREMENT,
    started_at DATETIME,
    ended_at DATETIME,
    success TINYINT,
    party_leader_char_id BIGINT
);

-- Optional mapping of custom enchanting items (IDs editable without recompile)
CREATE TABLE IF NOT EXISTS raid_config (
    cfg_key VARCHAR(64) PRIMARY KEY,
    cfg_value VARCHAR(255) NOT NULL
);