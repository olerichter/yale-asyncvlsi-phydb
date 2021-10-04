#ifndef PHYDB_INCLUDE_TECH_H_
#define PHYDB_INCLUDE_TECH_H_

#include <list>
#include <memory>
#include <unordered_map>

#include "layer.h"
#include "lefvia.h"
#include "macro.h"
#include "site.h"
#include "techconfig.h"
#include "viarulegenerate.h"

namespace phydb {

class Tech {
    friend class PhyDB;
  private:
    std::string version_;
    std::string bus_bit_char_;
    std::string divier_char_;
    std::string clearance_measure_;
    double manufacturing_grid_ = -1;

    int database_micron_ = -1;

    std::vector<Site> sites_;
    std::vector<Layer> layers_;
    std::vector<Macro> macros_;
    std::vector<LefVia> vias_;
    std::vector<ViaRuleGenerate> via_rule_generates_;

    std::unordered_map<std::string, int> layer_2_id_;
    std::unordered_map<std::string, int> macro_2_id_;
    std::unordered_map<std::string, int> via_2_id_;
    std::unordered_map<std::string, int> via_rule_generate_2_id_;

    /****placement grid parameters****/
    bool is_placement_grid_set_ = false;
    double placement_grid_value_x_ = -1;
    double placement_grid_value_y_ = -1;

    /****N/P-well layers_****/
    bool is_n_well_layer_set_ = false;
    bool is_p_well_layer_set_ = false;
    WellLayer *n_layer_ptr_ = nullptr;
    WellLayer *p_layer_ptr_ = nullptr;
    double same_diff_spacing_ = -1;
    double any_diff_spacing_ = -1;
    std::list<MacroWell> wells_;

    /****LEF file name****/
    std::string lef_name_;

    /****technology configuration file****/
    TechConfig tech_config_;
    std::vector<std::shared_ptr<Layer>> metal_layers_;

  public:
    Tech() : manufacturing_grid_(-1), database_micron_(-1) {}
    ~Tech();

    void SetDatabaseMicron(int database_micron);
    int GetDatabaseMicron() const;
    void SetManufacturingGrid(double manufacture_grid);
    double GetManufacturingGrid() const;
    void AddSite(
        std::string const &name,
        std::string const &class_name,
        double width,
        double height
    );
    std::vector<Site> &GetSitesRef();
    void SetPlacementGrids(
        double placement_grid_value_x,
        double placement_grid_value_y
    );
    bool GetPlacementGrids(
        double &placement_grid_value_x,
        double &placement_grid_value_y
    );

    bool IsLayerExisting(std::string const &layer_name);
    Layer *AddLayer(
        std::string &layer_name,
        LayerType type,
        MetalDirection direction = HORIZONTAL
    );
    Layer *GetLayerPtr(std::string const &layer_name);
    int GetLayerId(string const &layer_name);
    const std::string &GetLayerName(int layerID);
    std::vector<Layer> &GetLayersRef();

    bool IsMacroExisting(std::string const &macro_name);
    Macro *AddMacro(std::string &macro_name);
    Macro *GetMacroPtr(std::string const &macro_name);
    std::vector<Macro> &GetMacrosRef();

    bool IsLefViaExisting(std::string const &via_name);
    LefVia *AddLefVia(std::string &via_name);
    LefVia *GetLefViaPtr(std::string const &via_name);
    vector<LefVia> &GetLefViasRef();

    bool IsViaRuleGenerateExisting(std::string const &name);
    ViaRuleGenerate *AddViaRuleGenerate(std::string &name);
    ViaRuleGenerate *GetViaRuleGeneratePtr(std::string const &name);
    vector<ViaRuleGenerate> &GetViaRuleGeneratesRef();

    void SetNwellLayer(
        double width,
        double spacing,
        double op_spacing,
        double max_plug_dist,
        double overhang
    );
    void SetPwellLayer(
        double width,
        double spacing,
        double op_spacing,
        double max_plug_dist,
        double overhang
    );
    void SetNpwellSpacing(double same_diff, double any_diff);
    bool IsWellInfoSet() const;
    WellLayer *GetNwellLayerPtr();
    WellLayer *GetPwellLayerPtr();
    void GetDiffWellSpacing(double &same_diff_spacing, double any_diff_spacing);
    void ReportWellShape(); // report the well shape_ for each BlockType for debugging purposes.

    std::string GetLefName() const;
    void SetLefName(std::string const &lef_file_name);

    void FindAllMetalLayers();
    TechConfig &GetTechConfigRef();
    void AddTechConfigCorner(int corner_index);
    void AddResOverTable(ResOverTable *res_over_table);
    void AddCapOverTable(CapOverTable *cap_over_table);
    void AddCapUnderTable(CapUnderTable *cap_under_table);
    void AddCapDiagUnderTable(CapDiagUnderTable *cap_diag_under_table);
    void AddCapOverUnderTable(CapOverUnderTable *cap_over_under_table);
    void FixResOverTable();
    void SetResistanceUnit(bool from_tech_config, bool is_report);
    void SetCapacitanceUnit(bool from_tech_config, bool is_report);
    void ReportLayersTechConfig();

    void ReportSites();
    void ReportLayers();
    void ReportVias();
    void ReportMacros();
    void ReportMacroWell();
    void Report(); // for debugging purposes

    friend ostream &operator<<(ostream &, const Tech &);
};

ostream &operator<<(ostream &, const Tech &);

}

#endif //PHYDB_INCLUDE_TECH_H_
