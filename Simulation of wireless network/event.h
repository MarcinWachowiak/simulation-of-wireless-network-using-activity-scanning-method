#ifndef EVENT_H_
#define EVENT_H_

//parent class of all events in simulation
//mentions basic handles to check if event
//is triggered by time or conditions and execute it

class Simulation;

class Event
{
public:
	explicit Event(int time, 
		Simulation* simulation);
	
	virtual ~Event();
	
	virtual void execute() = 0;
	virtual bool isTriggered();
	bool isActive();

	//setter and getter
	int& get_time();
	void set_time(int time);
private:

protected:
	int time_;
;
protected:
	Simulation* simulation_;
	
};

#endif

