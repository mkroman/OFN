CREATE TABLE IF NOT EXISTS `postings` (
  `id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `nick` CHAR(31) NOT NULL,
  `user` CHAR(64) NOT NULL,
  `host` CHAR(255) NOT NULL,
  `url` CHAR(1024) NOT NULL,
  `created_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS `images` (
  `id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `digest` CHAR(64) UNIQUE NOT NULL,
  `filename` CHAR(255) NOT NULL,
  `posting_id` INTEGER
);

CREATE TABLE IF NOT EXISTS `signatures` (
  `id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `image_id` INTEGER NOT NULL,
  `compressed_signature` BLOB(182) UNIQUE NOT NULL
);

CREATE TABLE IF NOT EXISTS `words` (
  `pos_and_word` BLOB(5) NOT NULL,
  `signature_id` INTEGER NOT NULL
);

CREATE INDEX IF NOT EXISTS idx_pos_words ON words(pos_and_word);
CREATE INDEX IF NOT EXISTS idx_image_digest ON images(digest);
