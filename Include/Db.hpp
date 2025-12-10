#pragma once

#include <string>
#include <vector>

struct HighscoreRow {
    std::string name;   // player name (for UI compatibility)
    int         score;
    std::string when;   // timestamp / date string (can be empty for now)
    std::string mode;   // e.g. "classic", "survival"
};

namespace db {

    // Reads host/user/password/schema/port from cfg and tests connection.
    // Returns "OK" on success, or an error message string.
    std::string connect_from_cfg(const char* filename);

    // New API: explicit mode
    std::string upsert_player_and_add_score(const std::string& player_name,
        int score,
        const std::string& mode);

    // Legacy API for existing code that only passes (name, score).
    // Uses a default mode string.
    std::string upsert_player_and_add_score(const std::string& player_name,
        int score);

    // Get top N scores for a given mode.
    std::vector<HighscoreRow> top_scores(int limit,
        const std::string& mode);
}
