--
DROP TABLE IF EXISTS `mod_contributors_accounts`;
CREATE TABLE `mod_contributors_accounts` (
    `AccountId` INT UNSIGNED NOT NULL,
    `GrantedDate` INT UNSIGNED NOT NULL DEFAULT 0,
    `EndDate` INT UNSIGNED NOT NULL DEFAULT 0,
    `Level` TINYINT UNSIGNED NOT NULL DEFAULT 1,
    `Permanent` TINYINT NOT NULL DEFAULT 0,
    `Expired` TINYINT NOT NULL DEFAULT 0,
    PRIMARY KEY (`AccountId`)
);
