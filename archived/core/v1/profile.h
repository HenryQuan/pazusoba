/**
 * profile.h
 * P&D has many teams and profiles are for different teams and play style
 *
 * by Yiheng Quan
 */

#ifndef PROFILE_H
#define PROFILE_H

#include <vector>
#include <set>
#include <map>
#include <cmath>
#include <string>
#include <iostream>
#include "board.h"
#include "pad.h"
#include "configuration.h"
extern "C"
{

    // The base of all profiles, it only requires a getScore function
    class Profile
    {
    public:
        int row = Configuration::shared().getRow();
        int column = Configuration::shared().getColumn();
        int minErase = Configuration::shared().getMinErase();
        int maxStep = Configuration::shared().getMaxStep();

        virtual ~Profile() {}
        virtual std::string getProfileName() const = 0;
        virtual int getScore(const ComboList &list, const Board &board, int moveCount, int step) const = 0;
        virtual int getWeight() const = 0;
    };

    // This is a singleton class to update profiles at run time
    class ProfileManager
    {
        std::vector<Profile *> profiles;
        ProfileManager() {}

    public:
        // Return a shared instance of profile manager
        static ProfileManager &shared()
        {
            static ProfileManager p;
            return p;
        }

        // Loop through all profiles and add scores based on every profile
        // You need a list of combos, the current board and moveCount
        int getScore(const ComboList &list, const Board &board, int moveCount, int step)
        {
            int score = 0;
            int totalWeight = 0;

            for (auto &p : profiles)
            {
                // TODO: consider about weight
                int weight = 1;
                score += p->getScore(list, board, moveCount, step) * weight;
                totalWeight += weight;
            }

            // Get the average of all profiles so that they are equally important
            if (totalWeight == 0)
                return 0;
            return score;
        }

        void clear()
        {
            // Delete all profiles
            for (const auto &p : profiles)
            {
                delete p;
            }
            profiles.clear();
        }

        // Add a profile to current list
        void addProfile(Profile *p)
        {
            profiles.push_back(p);
        }

        // Clear all profiles and set it to a new one
        void updateProfile(const std::vector<Profile *> &p)
        {
            clear();
            for (const auto &p : profiles)
            {
                delete p;
            }
            profiles.clear();
            profiles = p;
        }
    };

    // More points for more combos
    class ComboProfile : public Profile
    {
        // If it is-1, always aim for more combo
        int targetCombo = -1;

    public:
        // Just a default profile
        ComboProfile() {}
        // With a target combo
        ComboProfile(int combo) : targetCombo(combo) {}

        std::string getProfileName() const override
        {
            return "combo";
        }

        int getWeight() const override
        {
            return 20;
        }

        int getScore(const ComboList &list, const Board &board, int moveCount, int step) const override
        {
            int score = 0;
            int combo = list.size();
            std::array<std::array<int, 2>, pad::ORB_COUNT> orbDistance{0};

            // std::array<Orb, pad::ORB_COUNT> orbInfo;
            for (int i = 0; i < row; i++)
            {
                for (int j = 0; j < column; j++)
                {
                    auto orb = board[INDEX_OF(i, j)];
                    // Store the max and min value of the X value of this orb
                    if (j < orbDistance[orb][0])
                    {
                        orbDistance[orb][0] = j;
                    }
                    if (j > orbDistance[orb][1])
                    {
                        orbDistance[orb][1] = j;
                    }
                }
            }

            if (targetCombo == 0)
            {
                // Aim for zero combo and make sure orbs are not close to each other
                score -= pad::TIER_EIGHT_SCORE * moveCount;
                score -= pad::TIER_TEN_SCORE * combo;
                for (const auto &orbInfo : orbDistance)
                {
                    score += (orbInfo[1] - orbInfo[0]) * 50;
                }
            }
            else
            {
                if (targetCombo > 0)
                {
                    int distance = abs(combo - targetCombo);
                    if (distance > 0)
                    {
                        // 1500 - 1700
                        score -= distance * 4650;
                    }
                }
                else
                {
                    for (const auto &combo : list)
                    {
                        if ((int)combo.size() > minErase)
                            score -= 100;
                    }
                }

                // 45 - 48 are the best score for now
                for (const auto &orbInfo : orbDistance)
                {
                    score -= (orbInfo[1] - orbInfo[0]) * 75;
                }

                // If the max step is small, no need to wait for combo
                if (maxStep <= 30 || step * 2 >= maxStep)
                    score += pad::TIER_TEN_SCORE * combo;
            }

            return score;
        }
    };

    // 7 Combo and less than 3 orbs remaining
    class AmenProfile : public Profile
    {
        int targetCombo = 7;
        int targetRemaining = 3;

    public:
        AmenProfile() {}
        AmenProfile(int c, int o) : targetCombo(c), targetRemaining(o) {}

        std::string getProfileName() const override
        {
            return "amen";
        }

        int getWeight() const override
        {
            return 100;
        }

        int getScore(const ComboList &list, const Board &board, int moveCount, int step) const override
        {
            // evaluate=-30*|7-コンボ数|-Σ(r_i-l_i)-10*(残りドロップ数-3) by koduma san
            std::array<std::array<int, 2>, pad::ORB_COUNT> orbDistance{0};
            // if (board[0] > 0)
            // {
            //     int a = 0;
            //     a++;
            // }

            int score = 0;
            int combo = list.size();
            // if (combo > targetCombo)
            //     combo = 0;
            score += combo * 1000;
            score += 50 * moveCount;

            int remaining = 0;
            int total = 0;
            for (int i = 0; i < row; i++)
            {
                for (int j = 0; j < column; j++)
                {
                    auto orb = board[INDEX_OF(i, j)];
                    if (orb != pad::empty)
                        remaining++;
                    total++;

                    // Store the max and min value of the X value of this orb
                    if (j < orbDistance[orb][0])
                    {
                        orbDistance[orb][0] = j;
                    }
                    if (j > orbDistance[orb][1])
                    {
                        orbDistance[orb][1] = j;
                    }
                }
            }

            // for (const auto &orbInfo : orbDistance)
            // {
            //     // printf("%d\n", orbInfo[1] - orbInfo[0]);
            //     score -= (orbInfo[1] - orbInfo[0]) * 500;
            // }

            // int erased = total - remaining;
            // if (remaining <= targetRemaining)
            //     erased = total - targetRemaining;
            // score += erased * 500;
            return score;
        }
    };

    // More points for more colour erased
    class ColourProfile : public Profile
    {
        // By default, all main colours
        std::vector<Orb> orbs;

    public:
        ColourProfile() {}
        ColourProfile(std::vector<Orb> o) : orbs(o) {}

        std::string getProfileName() const override
        {
            return "colour";
        }

        int getWeight() const override
        {
            return 8;
        }

        int getScore(const ComboList &list, const Board &board, int moveCount, int step) const override
        {
            int score = 0;
            std::set<Orb> colours;
            for (const auto &c : list)
            {
                auto orb = c[0].orb;
                if (orbs.size() == 0)
                {
                    colours.insert(orb);
                }
                else
                {
                    for (const auto &o : orbs)
                    {
                        if (orb == o)
                        {
                            // Only insert if matches
                            colours.insert(orb);
                            break;
                        }
                    }
                }
            }

            // Check if colours matches
            score += colours.size() * pad::TIER_SIX_SCORE;
            return score;
        }
    };

    // More points if a combo has a certain shape
    class ShapeProfile : public Profile
    {
        std::vector<Orb> orbs;

    public:
        ShapeProfile() {}
        ShapeProfile(std::vector<Orb> o) : orbs(o) {}

        // Check if orb list contains this orb
        virtual bool isTheOrb(Orb orb) const
        {
            if (orbs.size() == 0)
                return true;

            for (const auto &o : orbs)
            {
                if (orb == o)
                {
                    return true;
                }
            }

            return false;
        }
    };

    class TwoWayProfile : public ShapeProfile
    {
    public:
        TwoWayProfile() : ShapeProfile() {}
        TwoWayProfile(std::vector<Orb> orbs) : ShapeProfile(orbs) {}

        std::string getProfileName() const override
        {
            return "2U";
        }

        int getWeight() const override
        {
            return 3;
        }

        int getScore(const ComboList &list, const Board &board, int moveCount, int step) const override
        {
            int score = 0;
            for (const auto &c : list)
            {
                // recovery is not two way
                if (c[0].orb == pad::recovery)
                    continue;

                // 2U needs 4 orbs connected
                if (c.size() == 4 && isTheOrb(c[0].orb))
                {
                    score += pad::TIER_TEN_SCORE;
                }
            }
            return score;
        }
    };

    class LProfile : public ShapeProfile
    {
    public:
        LProfile() : ShapeProfile() {}
        LProfile(std::vector<Orb> orbs) : ShapeProfile(orbs) {}

        std::string getProfileName() const override
        {
            return "L";
        }

        int getWeight() const override
        {
            return 3;
        }

        int getScore(const ComboList &list, const Board &board, int moveCount, int step) const override
        {
            int score = 0;
            for (const auto &c : list)
            {
                // L shape must erase 5 orbs and right colour
                if (c.size() == 5 && isTheOrb(c[0].orb))
                {
                    std::map<int, int> vertical;
                    std::map<int, int> horizontal;
                    int bigFirst = -1;
                    int bigSecond = -1;

                    // Collect info
                    for (const auto &loc : c)
                    {
                        int x = loc.first;
                        int y = loc.second;
                        vertical[x]++;
                        horizontal[y]++;

                        // Track the largest number
                        if (vertical[x] >= 3)
                            bigFirst = x;
                        if (horizontal[y] >= 3)
                            bigSecond = y;
                    }

                    // This is the center point
                    if (bigFirst > -1 && bigSecond > -1)
                    {
                        int counter = 0;
                        // Check if bigFirst -2 or +2 exists
                        if (vertical[bigFirst - 2] > 0 || vertical[bigFirst + 2] > 0)
                            counter++;
                        // Same for bigSecond
                        if (horizontal[bigSecond - 2] > 0 || horizontal[bigSecond + 2] > 0)
                            counter++;

                        if (counter == 2)
                            score += pad::TIER_TEN_SCORE * 2;
                    }
                }
            }
            return score;
        }
    };

    class PlusProfile : public ShapeProfile
    {
    public:
        PlusProfile() : ShapeProfile() {}
        PlusProfile(std::vector<Orb> orbs) : ShapeProfile(orbs) {}

        std::string getProfileName() const override
        {
            return "+";
        }

        int getWeight() const override
        {
            return 3;
        }

        int getScore(const ComboList &list, const Board &board, int moveCount, int step) const override
        {
            int score = 0;
            for (const auto &c : list)
            {
                // + shape must erase 5 orbs
                if (c.size() == 5 && isTheOrb(c[0].orb))
                {
                    std::map<int, int> vertical;
                    std::map<int, int> horizontal;
                    int bigFirst = -1;
                    int bigSecond = -1;

                    // Collect info
                    for (const auto &loc : c)
                    {
                        int x = loc.first;
                        int y = loc.second;
                        vertical[x]++;
                        horizontal[y]++;

                        // Track the largest number
                        if (vertical[x] >= 3)
                            bigFirst = x;
                        if (horizontal[y] >= 3)
                            bigSecond = y;
                    }

                    // This is the center point
                    if (bigFirst > -1 && bigSecond > -1)
                    {
                        int counter = 0;
                        // Check up down left right there is an orb around center orb
                        if (vertical[bigFirst - 1] > 0 && vertical[bigFirst + 1] > 0)
                            counter++;
                        if (horizontal[bigSecond - 1] > 0 && horizontal[bigSecond + 1] > 0)
                            counter++;

                        if (counter == 2)
                            score += pad::TIER_TEN_SCORE * 2;
                    }
                }
            }
            return score;
        }
    };

    // Void damage penetration 無効貫通
    class VoidPenProfile : public ShapeProfile
    {
    public:
        VoidPenProfile() : ShapeProfile() {}
        VoidPenProfile(std::vector<Orb> orbs) : ShapeProfile(orbs) {}

        std::string getProfileName() const override
        {
            return "void damage penetration";
        }

        int getWeight() const override
        {
            return 10;
        }

        int getScore(const ComboList &list, const Board &board, int moveCount, int step) const override
        {
            // Try this board
            // DHLHHDHDDDHLDDHDLLLHHLHLLLLDHLHLDLHLLLHLHH
            int score = 0;
            for (const auto &c : list)
            {
                auto orb = c[0].orb;
                // More points if it is a 3x3 shape
                if (isTheOrb(orb))
                {
                    int size = c.size();
                    int distance = size - 9;

                    if (size == minErase)
                    {
                        score -= pad::TIER_NINE_SCORE;
                    }

                    if (size <= 9)
                    {
                        // Must connect more than min erase condition
                        score += size * pad::TIER_SEVEN_SCORE;

                        // Do the same like + and L
                        std::map<int, int> vertical;
                        std::map<int, int> horizontal;

                        // Collect info
                        for (const auto &loc : c)
                        {
                            int x = loc.first;
                            int y = loc.second;
                            vertical[x]++;
                            horizontal[y]++;
                        }

                        int v = vertical.size();
                        int h = horizontal.size();
                        if (v < 4 && h < 4)
                            score += (v + h) * pad::TIER_SEVEN_SCORE;

                        // All x and y are 3 because it is 3x3
                        if (v == 3 && h == 3)
                        {
                            int count = 0;
                            for (auto curr = vertical.begin(); curr != vertical.end(); curr++)
                            {
                                auto value = curr->second;
                                if (value == 3)
                                    count++;
                            }
                            for (auto curr = horizontal.begin(); curr != horizontal.end(); curr++)
                            {
                                auto value = curr->second;
                                if (value == 3)
                                    count++;
                            }

                            if (count < 6)
                                score += count * pad::TIER_EIGHT_PLUS_SCORE;
                            if (count == 6)
                                score += count * pad::TIER_NINE_SCORE;
                        }
                    }
                    else
                    {
                        score -= distance * pad::TIER_NINE_SCORE;
                    }
                }
            }
            return score;
        }
    };

    // Connect 10 - 12 orbs together and you get +1 combo and a green soybean
    class SoybeanProfile : public ShapeProfile
    {
    public:
        SoybeanProfile() : ShapeProfile() {}
        SoybeanProfile(std::vector<Orb> orbs) : ShapeProfile(orbs) {}

        std::string getProfileName() const override
        {
            return "soybean";
        }

        int getWeight() const override
        {
            return 8;
        }

        int getScore(const ComboList &list, const Board &board, int moveCount, int step) const override
        {
            int score = 0;
            for (const auto &c : list)
            {
                int size = c.size();
                if (size >= 10 && size <= 12 && isTheOrb(c[0].orb))
                {
                    score += pad::TIER_NINE_SCORE;
                }
            }
            return score;
        }
    };

    class OneRowProfile : public ShapeProfile
    {
    public:
        OneRowProfile() : ShapeProfile() {}
        OneRowProfile(std::vector<Orb> orbs) : ShapeProfile(orbs) {}

        std::string getProfileName() const override
        {
            return "row";
        }

        int getWeight() const override
        {
            return 8;
        }

        int getScore(const ComboList &list, const Board &board, int moveCount, int step) const override
        {
            int score = 0;
            for (const auto &c : list)
            {
                int size = c.size();
                // It has a row, xxxxxx or more
                if (size == row && isTheOrb(c[0].orb))
                {
                    // Record all encountered x
                    std::map<int, int> xs;
                    for (const auto &loc : c)
                    {
                        // Track x because it should have the number of row
                        xs[loc.first]++;
                    }

                    // Sometimes, it is a long L so just check which x has more than row times
                    for (auto curr = xs.begin(); curr != xs.end(); curr++)
                    {
                        if (curr->second == row)
                        {
                            score += pad::TIER_NINE_SCORE;
                            break;
                        }
                    }
                }
            }
            return score;
        }
    };

    class OneColumnProfile : public ShapeProfile
    {
    public:
        OneColumnProfile() : ShapeProfile() {}
        OneColumnProfile(std::vector<Orb> orbs) : ShapeProfile(orbs) {}

        std::string getProfileName() const override
        {
            return "column";
        }

        int getWeight() const override
        {
            return 10;
        }

        int getScore(const ComboList &list, const Board &board, int moveCount, int step) const override
        {
            int score = 0;
            for (const auto &c : list)
            {
                int size = c.size();
                // It is a column
                if (size == column && isTheOrb(c[0].orb))
                {
                    // Record all encountered x
                    std::map<int, int> xs;
                    for (const auto &loc : c)
                    {
                        // Track x because it should have the number of row
                        xs[loc.second]++;
                    }

                    // Sometimes, it is a long L so just check which x has more than row times
                    for (auto curr = xs.begin(); curr != xs.end(); curr++)
                    {
                        if (curr->second == column)
                        {
                            score += pad::TIER_EIGHT_PLUS_SCORE;
                            break;
                        }
                    }
                }
            }
            return score;
        }
    };

    // More points if there are less orbs left
    class OrbProfile : public Profile
    {
        // Aim for nothing less
        int targetNumber = 0;

    public:
        OrbProfile() {}
        OrbProfile(int count) : targetNumber(count) {}

        std::string getProfileName() const override
        {
            return "orb remains";
        }

        int getWeight() const override
        {
            return 10;
        }

        int getScore(const ComboList &list, const Board &board, int moveCount, int step) const override
        {
            int score = 0;
            if (list.size() == 0)
                return score;

            // Track how many orbs are erased and left
            int orbErased = 0;
            // Encourage to connect more orbs in a combo
            for (const auto &c : list)
            {
                int size = c.size();
                score += (size - minErase) * 80;
                orbErased += size;
            }

            int orbLeft = board.size() - orbErased;
            int ok = orbLeft - targetNumber;
            // As long as, we reached the goal, don't bother erase more
            if (ok <= 0)
                orbErased += ok;
            score += orbErased * 3000;

            return score;
        }
    };
}
#endif
