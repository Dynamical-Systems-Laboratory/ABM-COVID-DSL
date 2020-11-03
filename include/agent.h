#ifndef AGENT_H
#define AGENT_H

#include "common.h"
#include "infection.h"

class Infection;

/***************************************************** 
 * class: Agent
 * 
 * Defines and stores attributes of a single agent
 * 
 *****************************************************/

class Agent{
public:
	
	//
	// Constructors
	//

	/**
	 * \brief Creates an Agent object with default attributes
	 */
	Agent() = default;

	/**
 	 * \brief Creates an Agent object with custom attributes
 	 *
 	 * @param student - true (marked as 1) if Agent is a student
 	 * @param works - true (marked as 1) if Agent works
 	 * @param yrs - age of the Agent
 	 * @param xi - x coordinate of the Agent
 	 * @param yi - y coordinate of the Agent
 	 * @param houseID - household ID
	 * @param isPatient - true if hospitalized with a condition other than covid
 	 * @param schoolID - ID of the school Agent attends
	 * @param lvRH - true if agent lives at a retirement home
	 * @param wrkRH - true if agent works at a retirement home
	 * @param wrkSch - true if agent works at a school
	 * @param workID - ID of the workplace Agent works at
	 * @param worksHospital - true if agent works at a hospital
	 * @param HospitalID - ID of the hospital where agent is staff or patient
 	 * @param infected - true if Agent is infected
 	 */	
	Agent(const bool student, const bool works, const int yrs, const double xi, 
			const double yi, const int houseID, const bool isPatient, const int schoolID, 
			const bool lvRH, const bool wrkRH, const bool wrkSch, const int workID, 
			const bool worksHospital, const int hospitalID, const bool infected) 
			: is_student(student), is_working(works), age(yrs),
		   		x(xi), y(yi), house_ID(houseID), is_non_covid_patient(isPatient), school_ID(schoolID), 
				livesRH(lvRH), worksRH(wrkRH), worksSch(wrkSch), work_ID(workID),  
				works_at_hospital(worksHospital), hospital_ID(hospitalID), is_infected(infected) { }  

	//
	// Infection related computations
	//

	//
	// Getters
	//

	/// Retrieve this agents ID
	int get_ID() const { return ID; }
	/// Agents age
	int get_age() const { return age; }
	/// House ID
	int get_household_ID() const { return house_ID; }
	/// School ID
	int get_school_ID() const { return school_ID; }
	/// Work ID
	int get_work_ID() const { return work_ID; }
	/// Hospital ID if staff or patient
	int get_hospital_ID() const { return hospital_ID; }

	/// Location - x coordinates
	double get_x_location() const { return x; }	
	/// Location - y coordinates
	double get_y_location() const { return y; }

	/// True if infected
	bool infected() const { return is_infected; }
	/// True if student
	bool student() const { return is_student; }
	/// True if agent works
	bool works() const { return is_working; }
	/// True if agent works at a hospital
	bool hospital_employee() const { return works_at_hospital; }
	/// True if agent is a hospital patient with condition other than COVID
	bool hospital_non_covid_patient() const { return is_non_covid_patient; }
	/// True if agent works in a retirement home 
	bool retirement_home_employee() const { return worksRH; }
	/// True if agent works at a school
	bool school_employee() const { return worksSch; }
	/// True if agent lives in a retirement home 
	bool retirement_home_resident() const { return livesRH; }

	/// State getters
	bool exposed() const { return is_exposed; }
	bool recovering_exposed() const { return is_recovering_exposed; }
	bool symptomatic() const { return is_symptomatic; }
	bool symptomatic_non_covid() const { return is_symptomatic_non_covid;}
	// Testing results
	bool tested_covid_negative() const { return is_tested_covid_negative; }
	bool tested_false_negative() const { return is_tested_false_negative; }
	bool tested_false_positive() const { return is_tested_false_positive; }
	bool tested_covid_positive() const { return is_tested_covid_positive; }
	// Testing phases and types 
	bool tested() const { return is_tested; }
	bool tested_exposed() const { return is_tested_exposed; }
	bool tested_in_car() const { return is_tested_in_car; }
	bool tested_in_hospital() const { return is_tested_in_hospital; }
	bool tested_awaiting_results() const { return is_tested_awaiting_results; }
	bool tested_awaiting_test() const { return is_tested_awaiting_test; } 
	double get_time_for_flu_isolation() { return time_flu_ih; }
	bool get_testing_since_exposed() { return is_testing_since_exposed; }
	// Treatment types
	bool being_treated() const { return is_treated; }
	bool home_isolated() const { return is_home_isolated; }
	bool hospitalized() const { return is_hospitalized; }
	bool hospitalized_ICU() const { return is_hospitalized_ICU; }
	// Treatment - as set for regular agent
	bool get_will_be_hospitalized() const { return will_be_hospitalized; }
	bool get_will_be_hospitalized_ICU() const { return will_be_in_ICU; }
	bool get_will_be_home_isolated() const { return will_be_home_isolated; }
	// Removal
	bool dying() const { return will_die; }
	bool recovering() const { return will_recover; }
	bool removed() const { return is_removed; }
	bool vaccinated() const { return is_vaccinated; }

	/// Get infectiousness variability factor of an agent
	double get_inf_variability_factor() const { return inf_var; }
	/// Get latency end time
	double get_latency_end_time() const { return latency_end_time; }
	/// Time when the latent-non infectious period ends
	double get_infectiousness_start_time() const { return infectiousness_start; }
	/// Get time of death if not recovering
	double get_time_of_death() const { return death_time; }
	/// Get time of recovery
	double get_recovery_time() const { return recovery_time; } 
	/// Time of testing
	double get_time_of_test() const { return time_of_test; }
	/// Time when agent gets their testing results
	double get_time_of_results() const { return time_of_results; }
	/// Time when dying agent is transferred from hospital to ICU
	double get_time_hsp_to_icu() const { return time_hsp_to_ICU; }
	/// Time when agent is transferred from ICU to hospital
	double get_time_icu_to_hsp() const { return time_icu_to_hsp; }
	/// Time when recovering agent is transferred from hospital to home isolation
	double get_time_hsp_to_ih() const { return time_hsp_to_ih; }
	/// Time when dying agent is transferred from home isolation to ICU
	double get_time_ih_to_icu() const { return time_ih_to_icu; }
	/// Time when recovering agent is transferred from home isolation to hospital
	double get_time_ih_to_hsp() const { return time_ih_to_hsp; }

	//
	// Setters
	//

	/// Assign ID to an agent
	void set_ID(const int agent_ID) { ID = agent_ID; }	

	/// Assign hospital ID for testing
	void set_hospital_ID(const int ID) { hospital_ID = ID; }

	/// Assign household ID
	void set_household_ID(const int ID) { house_ID = ID; }

	/// Change infection status
	void set_infected(const bool infected) { is_infected = infected; }

	// Latency
	/// Set latency duration time
	void set_latency_duration(const double ltime) { latency_duration = ltime; }
	/// Compute latency end from current time
	void set_latency_end_time(const double cur_time) 
		{ latency_end_time = cur_time + latency_duration; }
	/// Set tme when the pre-infectious period ends
	void set_infectiousness_start_time(const double cur_time, const double dt) 
		{ infectiousness_start = cur_time + dt; }

	// Death 
	/// Set onset to death duration time
	void set_time_to_death(const double dtime) { otd_duration = dtime; }
	/// Compute death time from current time
	void set_death_time(const double cur_time) 
		{ death_time = cur_time + otd_duration; }

	// Recovery
	/// Set recovery duration time
	void set_recovery_duration(const double rtime) { recovery_duration = rtime; }
	/// Compute recovery end from current time
	void set_recovery_time(const double cur_time) 
		{ recovery_time = cur_time + recovery_duration; }

	// Testing
	void set_time_to_test(const double test_time) { time_to_test = test_time; }
	void set_time_of_test(const double cur_time) { time_of_test = cur_time + time_to_test; }

	// Test results
	void set_time_until_results(const double test_res_time) { time_until_results = test_res_time; }
	void set_time_of_results(const double cur_time) { time_of_results = cur_time + time_until_results; }

	/// Transition from hospital to ICU
	void set_time_hsp_to_icu(const double t_icu) { time_hsp_to_ICU = t_icu; }
	/// Transition from hospital to home isolation
	void set_time_hsp_to_ih(const double t_ih) { time_hsp_to_ih = t_ih; }
	/// Transition from ICU to hospital
	void set_time_icu_to_hsp(const double t_icu) { time_icu_to_hsp = t_icu; }
	/// Transition from home isolation to ICU
	void set_time_ih_to_icu(const double t_icu) { time_ih_to_icu = t_icu; }
	/// Transition from home isolation to hospital
	void set_time_ih_to_hsp(const double t_hsp) { time_ih_to_hsp = t_hsp; }

	/// State setters
	void set_exposed(const bool val) { is_exposed = val; }
	void set_recovering_exposed(const bool re) { is_recovering_exposed = re; }
	void set_symptomatic(const bool val) { is_symptomatic = val; }
	void set_symptomatic_non_covid(const bool val) { is_symptomatic_non_covid = val; }

	// Testing results
	void set_tested_covid_negative(const bool val) { is_tested_covid_negative = val; }
	void set_tested_false_negative(const bool val) { is_tested_false_negative = val; }
	void set_tested_false_positive(const bool val) { is_tested_false_positive = val; }
	void set_tested_covid_positive(const bool val) { is_tested_covid_positive = val; }
	// Testing phases and types
	void set_tested(const bool val) { is_tested = val; }
	void set_tested_in_car(const bool val) { is_tested_in_car = val; }
	void set_tested_in_hospital(const bool val) { is_tested_in_hospital = val; }
	void set_tested_awaiting_results(const bool val) { is_tested_awaiting_results = val; }
	void set_tested_awaiting_test(const bool val) { is_tested_awaiting_test = val; }
	void set_tested_exposed(const bool val) { is_tested_exposed = val; }
	void set_flu_isolation(const double val) { time_flu_ih = time_of_test - val; }
	void set_testing_since_exposed(const bool val) { is_testing_since_exposed = val;}

	// Treatment types
	void set_being_treated(const bool val) { is_treated = val; }
	void set_home_isolated(const bool val) { is_home_isolated = val; }
	void set_hospitalized(const bool val) { is_hospitalized = val; }
	void set_hospitalized_ICU(const bool val) { is_hospitalized_ICU = val; }
	void set_dying(const bool val) { will_die = val; }
	void set_recovering(const bool val) { will_recover = val; }
	void set_removed(const bool val) { is_removed = val; }
	void set_vaccinated(const bool val) { is_vaccinated = val; }
	// Treatment - as set for regular
	void to_be_hospitalized(const bool val) { will_be_hospitalized = val; }
	void to_be_in_ICU(const bool val) { will_be_in_ICU = val; }
	void to_be_home_isolated(const bool val) { will_be_home_isolated = val; }

	/// Set infectiousness variability factor of an agent
	void set_inf_variability_factor(const double var) { inf_var = var; }

	//
	// I/O
	//

	/**
	 * \brief Print agent information 
	 * \details The information is in the same order as in the constructor,
	 *		except that it is preceeded by agent ID set separately
	 * 	@param where - output stream
	 */	
	void print_basic(std::ostream& where) const;

private:

	// General demographic information
	bool is_student = false;
	bool is_working = false;
	int age = 0;

	// Latency duration in time
	double latency_duration = 0.0; 
	// Start of infectiousness
	double infectiousness_start = 0.0;
	// End of latency period within the simulation time
	double latency_end_time = 0.0;
	// Time from becoming symptomatic (onset) to death
	double otd_duration = 0.0;
	// Time of death
	double death_time = 0.0;
	// Time to recover
	double recovery_duration = 0.0;
	// Time of recovery
	double recovery_time = 0.0;
	// Time between testing decision and the test
	double time_to_test = 0.0;
	// Exact time of testing
	double time_of_test = 0.0;
	// Time from testing until results
	double time_until_results = 0.0;
	// Time when results are available
	double time_of_results = 0.0;
	// Time when dying agent is transferred from hospital to ICU
	double time_hsp_to_ICU = 0.0;
	// Time from hospitalization to home isolation
	double time_hsp_to_ih = 0.0;
	// Time from ICU to hospitalization
	double time_icu_to_hsp = 0.0;
	// Time from IH to ICU
	double time_ih_to_icu = 0.0;
	// Time from IH to hospitalization
	double time_ih_to_hsp = 0.0;
	// Start home isolation for flu agents
	// before getting tested
	double time_flu_ih = 0.0;

	// ID
	int ID = 0;

	// Location
	double x = 0.0, y = 0.0;

	// Household ID
	int house_ID = -1;
	// Hospital patient with something else than COVID
	bool is_non_covid_patient = false;

	// School and work IDs and types
	int school_ID = -1;
	int work_ID = -1;
	int hospital_ID = -1;
	int agent_school_type = -1; 
	bool works_at_hospital = false;
	bool worksRH = false;
	bool worksSch = false;
	bool livesRH = false;

	// Infection status
	bool is_infected = false;

	// Ratio of distances with infected and all distances
	double dist_ratio = 0.0;

	// State information
	bool is_exposed = false;
	// Recovering without ever developing symptoms
	bool is_recovering_exposed = false;
	bool is_symptomatic = false;
	bool is_symptomatic_non_covid = false;
	// Testing results
	bool is_tested_covid_negative = false;
	bool is_tested_false_negative = false;
	bool is_tested_false_positive = false;
	bool is_tested_covid_positive = false;
	// Testing phases and types
	bool is_tested = false;
	bool is_tested_in_car = false;
	bool is_tested_in_hospital = false;
	bool is_tested_awaiting_results = false;
	bool is_tested_awaiting_test = false;
	bool is_tested_exposed = false;
	bool is_testing_since_exposed = false;
	// Treatment types
	bool is_treated = false;
	bool is_home_isolated = false;
	bool is_hospitalized = false;
	bool is_hospitalized_ICU = false;
	bool will_die = false;
	bool will_recover = false;
	bool is_removed = false;
	bool is_vaccinated = false;
	bool will_be_hospitalized = false;
	bool will_be_in_ICU = false;
	bool will_be_home_isolated = false;

	// Infectiousness variability parameter
	double inf_var = -1.0;

	//
	// Private member functions
	//
	
	/** 
	 * \brief Function of distance for infection propagation
	 * @param a - Infection parameter for distance scaling, units of distance
	 * @param b - Infection parameter for distance scaling, unitless
     * @param dij - distance between agents i and j 
	 *
 	 * @returns Value of the distance function
	 */	
	double distance_function(const double a, const double b, const double dij) const;
};

/// Overloaded ostream operator for I/O
std::ostream& operator<< (std::ostream& out, const Agent& agent);

#endif
