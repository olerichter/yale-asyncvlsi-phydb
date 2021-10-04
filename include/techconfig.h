#ifndef PHYDB_INCLUDE_TECHCONFIG_H_
#define PHYDB_INCLUDE_TECHCONFIG_H_

#include <vector>

#include "scanner.h"
#include "parser.hpp"

namespace phydb {

/****
 * @brief This structure presents an entry in tables in a technology configuration
 * file from OpenRCX.
 *
 * This structure contains four attributes:
 * 1. distance_: distance of two metal segments.
 * 2. coupling_cap_: coupling capacitance for a unit length.
 * 3. fringe_cap_: capacitance to the substrate for a unit length.
 * 4. res_: resistance for a unit length.
 *
 * A unit length is usually 1 micrometer.
 */
struct CapEntry {
  public:
    CapEntry(
        double distance,
        double coupling_cap,
        double fringe_cap,
        double res
    ) :
        distance_(distance),
        coupling_cap_(coupling_cap),
        fringe_cap_(fringe_cap),
        res_(res) {}
    double distance_;
    double coupling_cap_;
    double fringe_cap_;
    double res_;

    void Report() const;
};

/****
 * @brief A structure containing a resistance table.
 *
 * This table contains the following for columns in a technology configuration
 * file: distance_, coupling_cap_, fringe_cap_, res_.
 * However, the fringe_cap_ is 0 for most of the time, and the coupling_cap_
 * refers to another distance instead of a capacitance. Here is what an entry
 * in this table really is:
 * distance_1, distance_2, 0, resistance
 */
struct ResOverTable {
  public:
    ResOverTable(int layer_index, int over_index) :
        layer_index_(layer_index),
        over_index_(over_index) {}

    void AddEntry(
        double distance,
        double coupling_cap,
        double fringe_cap,
        double res
    );

    void FixLastEntryIfWrong();
    void SetWidth(double width);
    double Width() const;
    int LayerIndex() const;
    int OverIndex() const;
    std::vector<CapEntry> &GetTable();

    void Report() const;
  private:
    double width_ = -1;
    int layer_index_;
    int over_index_;
    std::vector<CapEntry> table_;
};

/****
 * @brief A structure containing a table for storing capacitance if a model layer
 * is over another layer.
 *
 */
struct CapOverTable {
  public:
    CapOverTable(int layer_index, int over_index) :
        layer_index_(layer_index),
        over_index_(over_index) {}

    void AddEntry(
        double distance,
        double coupling_cap,
        double fringe_cap,
        double res
    );
    void SetWidth(double width);
    double Width() const;
    int LayerIndex() const;
    int OverIndex() const;
    std::vector<CapEntry> &GetTable();
    void Report() const;
  private:
    double width_ = -1;
    int layer_index_;
    int over_index_;
    std::vector<CapEntry> table_;
};

/****
 * @brief A structure containing a table for storing capacitance if a model layer
 * is under another layer.
 *
 */
class CapUnderTable {
  public:
    CapUnderTable(int layer_index, int under_index) :
        layer_index_(layer_index),
        under_index_(under_index) {}

    void AddEntry(
        double distance,
        double coupling_cap,
        double fringe_cap,
        double res
    );
    void SetWidth(double width);
    double Width() const;
    int LayerIndex() const;
    int UnderIndex() const;
    std::vector<CapEntry> &GetTable();
    void Report();
  private:
    double width_ = -1;
    int layer_index_;
    int under_index_;
    std::vector<CapEntry> table_;
};

/****
 * @brief A structure containing a table for storing capacitance if a model layer
 * is diagunder another layer.
 *
 */
class CapDiagUnderTable {
  public:
    CapDiagUnderTable(int layer_index, int diagunder_index) :
        layer_index_(layer_index),
        diagunder_index_(diagunder_index) {}

    void AddEntry(
        double distance,
        double coupling_cap,
        double fringe_cap,
        double res
    );
    void SetWidth(double width);
    double Width() const;
    int LayerIndex() const;
    int DiagUnderIndex() const;
    std::vector<CapEntry> &GetTable();
    void Report();
  private:
    double width_ = -1;
    int layer_index_;
    int diagunder_index_;
    std::vector<CapEntry> table_;
};

/****
 * @brief A structure containing a table for storing capacitance if a model layer
 * is over one layer and under another layer.
 */
class CapOverUnderTable {
  public:
    CapOverUnderTable(int layer_index, int over_index, int under_index) :
        layer_index_(layer_index),
        over_index_(over_index),
        under_index_(under_index) {}

    void AddEntry(
        double distance,
        double coupling_cap,
        double fringe_cap,
        double res
    );
    void SetWidth(double width);
    double Width() const;
    int LayerIndex() const;
    int OverIndex() const;
    int UnderIndex() const;
    std::vector<CapEntry> &GetTable();
    void Report();
  private:
    double width_ = -1;
    int layer_index_;
    int over_index_;
    int under_index_;
    std::vector<CapEntry> table_;
};

class TechConfig {
  public:
    bool is_diagmodel_on_ = false;
    int layer_count_ = 0;
    int model_count_; // DensityRate : model_count data_rate_table
    std::vector<double> data_rate_table_;

    void SetDiagModelOn(bool is_diagmodel_on);
    void SetLayerCount(int layer_count);
    void SetModelCount(int model_count);
    void AddDataRate(double data_rate);

    void Report();
};

}

#endif //PHYDB_INCLUDE_TECHCONFIG_H_
