//DumpPlanSummary.cc - A part of DICOMautomaton 2019. Written by hal clark.

#include <boost/interprocess/creation_tags.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <cmath>
#include <exception>
#include <experimental/any>
#include <experimental/optional>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <regex>
#include <stdexcept>
#include <string>    
#include <utility>            //Needed for std::pair.
#include <vector>

#include "../Structs.h"
#include "../Regex_Selectors.h"
#include "DumpPlanSummary.h"
#include "Explicator.h"       //Needed for Explicator class.
#include "YgorFilesDirs.h"    //Needed for Does_File_Exist_And_Can_Be_Read(...), etc..
#include "YgorImages.h"
#include "YgorMath.h"         //Needed for vec3 class.
#include "YgorMisc.h"         //Needed for FUNCINFO, FUNCWARN, FUNCERR macros.
#include "YgorStats.h"        //Needed for Stats:: namespace.



OperationDoc OpArgDocDumpPlanSummary(void){
    OperationDoc out;
    out.name = "DumpPlanSummary";

    out.desc = 
        "This operation dumps a summary of a radiotherapy plan. This operation can be used to gain insight into a plan"
        " from a high-level overview.";
        
        
    out.args.emplace_back();
    out.args.back().name = "SummaryFileName";
    out.args.back().desc = "A filename (or full path) in which to append summary data generated by this routine."
                           " The format is CSV. Leave empty to dump to generate a unique temporary file.";
    out.args.back().default_val = "";
    out.args.back().expected = true;
    out.args.back().examples = { "", "/tmp/somefile", "localfile.csv", "derivative_data.csv" };
    out.args.back().mimetype = "text/csv";


    out.args.emplace_back();
    out.args.back().name = "UserComment";
    out.args.back().desc = "A string that will be inserted into the output file which will simplify merging output"
                           " with differing parameters, from different sources, or using sub-selections of the data."
                           " If left empty, the column will be omitted from the output.";
    out.args.back().default_val = "";
    out.args.back().expected = true;
    out.args.back().examples = { "", "Using XYZ", "Patient treatment plan C" };


    return out;
}



Drover DumpPlanSummary(Drover DICOM_data, OperationArgPkg OptArgs, std::map<std::string,std::string> /*InvocationMetadata*/, std::string FilenameLex){

    //---------------------------------------------- User Parameters --------------------------------------------------
    auto SummaryFileName = OptArgs.getValueStr("SummaryFileName").value();
    const auto UserComment = OptArgs.getValueStr("UserComment");

    //-----------------------------------------------------------------------------------------------------------------
    struct multi_key {
        std::string plan;  // RTPlanLabel.
        std::string beam;  // BeamName.
        std::string param; // Static_Machine_State parameter being considered.

        bool operator<(const multi_key &rhs) const {
            if(this->plan != rhs.plan) return (this->plan < rhs.plan);
            if(this->beam != rhs.beam) return (this->beam < rhs.beam);
            return (this->param < rhs.param);
        }
    };
    std::map<multi_key, std::vector<double>> coords;


    for(auto &tp : DICOM_data.tplan_data){
        if(tp == nullptr) continue;

        multi_key k;
        k.plan = tp->metadata["RTPlanLabel"];

        for(auto &ds : tp->dynamic_states){
            k.beam = ds.metadata["BeamName"];

            for(auto &ss : ds.static_states){
                k.param = "CumulativeMetersetWeight";
                coords[k].emplace_back(ss.CumulativeMetersetWeight);

                k.param = "GantryAngle";
                coords[k].emplace_back(ss.GantryAngle);
                k.param = "GantryRotationDirection";
                coords[k].emplace_back(ss.GantryRotationDirection);

                k.param = "BeamLimitingDeviceAngle";
                coords[k].emplace_back(ss.BeamLimitingDeviceAngle);
                k.param = "BeamLimitingDeviceRotationDirection";
                coords[k].emplace_back(ss.BeamLimitingDeviceRotationDirection);

                k.param = "PatientSupportAngle";
                coords[k].emplace_back(ss.PatientSupportAngle);
                k.param = "PatientSupportRotationDirection";
                coords[k].emplace_back(ss.PatientSupportRotationDirection);

                k.param = "TableTopEccentricAngle";
                coords[k].emplace_back(ss.TableTopEccentricAngle);
                k.param = "TableTopEccentricRotationDirection";
                coords[k].emplace_back(ss.TableTopEccentricRotationDirection);

                k.param = "TableTopVerticalPosition";
                coords[k].emplace_back(ss.TableTopVerticalPosition);
                k.param = "TableTopLongitudinalPosition";
                coords[k].emplace_back(ss.TableTopLongitudinalPosition);
                k.param = "TableTopLateralPosition";
                coords[k].emplace_back(ss.TableTopLateralPosition);

                k.param = "TableTopPitchAngle";
                coords[k].emplace_back(ss.TableTopPitchAngle);
                k.param = "TableTopPitchRotationDirection";
                coords[k].emplace_back(ss.TableTopPitchRotationDirection);

                k.param = "TableTopRollAngle";
                coords[k].emplace_back(ss.TableTopRollAngle);
                k.param = "TableTopRollRotationDirection";
                coords[k].emplace_back(ss.TableTopRollRotationDirection);


                k.param = "IsocentrePositionX";
                coords[k].emplace_back(ss.IsocentrePosition.x);
                k.param = "IsocentrePositionY";
                coords[k].emplace_back(ss.IsocentrePosition.y);
                k.param = "IsocentrePositionZ";
                coords[k].emplace_back(ss.IsocentrePosition.z);


                for(size_t i = 0; i < ss.JawPositionsX.size(); ++i){
                    std::stringstream sstr;
                    sstr << "JawPositionsX_" << std::setw(3) << std::setfill('0') << std::to_string(i);
                    k.param = sstr.str();
                    coords[k].emplace_back(ss.JawPositionsX[i]);
                }
                for(size_t i = 0; i < ss.JawPositionsY.size(); ++i){
                    std::stringstream sstr;
                    sstr << "JawPositionsY_" << std::setw(3) << std::setfill('0') << std::to_string(i);
                    k.param = sstr.str();
                    coords[k].emplace_back(ss.JawPositionsY[i]);
                }
                for(size_t i = 0; i < ss.MLCPositionsX.size(); ++i){
                    std::stringstream sstr;
                    sstr << "MLCPositionsX_" << std::setw(3) << std::setfill('0') << std::to_string(i);
                    k.param = sstr.str();
                    coords[k].emplace_back(ss.MLCPositionsX[i]);
                }
            }
        }
    }

    //-----------------------------------------------------------------------------------------------------------------

    //Report the findings. 
    FUNCINFO("Attempting to claim a mutex");
    try{
        //File-based locking is used so this program can be run over many patients concurrently.
        //
        //Try open a named mutex. Probably created in /dev/shm/ if you need to clear it manually...
        boost::interprocess::named_mutex mutex(boost::interprocess::open_or_create,
                                               "dicomautomaton_operation_dumpplansummary_mutex");
        boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(mutex);

        if(SummaryFileName.empty()){
            SummaryFileName = Get_Unique_Sequential_Filename("/tmp/dicomautomaton_dumpplansummary_", 6, ".csv");
        }
        const auto FirstWrite = !Does_File_Exist_And_Can_Be_Read(SummaryFileName);
        std::fstream FO(SummaryFileName, std::fstream::out | std::fstream::app);
        if(!FO){
            throw std::runtime_error("Unable to open file for reporting summary data. Cannot continue.");
        }
        if(FirstWrite){ // Write a CSV header.
            FO << "UserComment,"
               << "Plan,"
               << "Beam,"
               << "Quantity,"
               << "Min,"
               << "Mean,"
               << "Max"
               << std::endl;
        }
        for(const auto &c : coords){
            const auto k = c.first;

            const auto PlanLabel = k.plan;
            const auto BeamLabel = k.beam;
            const auto Quantity = k.param;

            const auto Min = Stats::Min(c.second);
            const auto Mean = Stats::Mean(c.second);
            const auto Max = Stats::Max(c.second);

            // Note: limit output to two decimal digits for improved readability.
            FO << UserComment.value_or("") << ","
               << "'" << PlanLabel << "'" << ","
               << "'" << BeamLabel << "'" << ","
               << "'" << Quantity << "'" << ","
               << std::setprecision(2) << std::fixed << Min << ","
               << std::setprecision(2) << std::fixed << Mean << ","
               << std::setprecision(2) << std::fixed << Max << std::endl;
        }
        FO.flush();
        FO.close();

    }catch(const std::exception &e){
        FUNCERR("Unable to write to log files: '" << e.what() << "'");
    }

    return DICOM_data;
}
