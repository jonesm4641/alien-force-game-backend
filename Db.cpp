#include "Db.hpp"
#include <vector>
#include <string>

namespace db {

    // Just pretend the config worked; no real DB yet.
    std::string connect_from_cfg(const char* /*filename*/)
    {
        // Return empty string = "no error" for your main.cpp check
        return {};
    }

    // 3-arg version – currently a no-op
    std::string upsert_player_and_add_score(const std::string& player_name,
        int score,
        const std::string& mode)
    {
        (void)player_name;
        (void)score;
        (void)mode;
        // No DB error
        return {};
    }

    // 2-arg legacy version – just forwards to 3-arg
    std::string upsert_player_and_add_score(const std::string& player_name,
        int score)
    {
        return upsert_player_and_add_score(player_name, score, "demo");
    }

    // Return an empty scoreboard for now
    std::vector<HighscoreRow> top_scores(int limit,
        const std::string& mode)
    {
        (void)limit;
        (void)mode;
        return {};
    }

} // namespace db
