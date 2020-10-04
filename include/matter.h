#include <iostream>
#include "vector3.h"
#include <math.h>
#include "constant.h"

using namespace std;

class Matter{
private:
    double mass;
    double time;
	double temperature;
	double emissivity;
	double radius;
	bool blackBody;
	bool blackHole;

    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
	double gamma;

    double dt;
public:
    Matter(){
        this->setMass();
		this->setEmissivity(1);
		blackBody = false;
		this->setTemperature(273);
		this->blackHole = false;

        time=0.0;
        dt=0;
    }

    void operator = (const Matter &m) {
        this->mass=m.mass;
        this->time=m.time;
        this->position=m.position;
        this->velocity=m.velocity;
        this->acceleration=m.acceleration;
		this->gamma = m.gamma;
		this->radius = m.radius;
		this->temperature = m.temperature;
		this->emissivity = m.emissivity;
		this->blackBody = m.blackBody;
		this->blackHole = m.blackHole;

    }

    //Setting values
    void setMass(double mass=1){
        this->mass=mass;
		this->setBlackHole();

    }
    void setPosition(Vector3 position){
       this->position.setValue(position);
    }
    void setVelocity(Vector3 velocity){
        this->velocity.setValue(velocity);
    }
	void setRadius(double radius) {
		this->radius = radius;
		this->setBlackHole();

	}
	void setEmissivity(double emissivity) {
		this->emissivity = emissivity;
	}
	void setTemperature(double temp) {
		this->temperature = temp;
	}
	void setBlackBody(bool isBlackBody) {
		this->blackBody = isBlackBody;
	}

	double getRadiationWaveLength(){
        return 2.897e6/this->temperature;
	}

	void setBlackHole() {
		if ((2 * G*this->mass) / (C*C) > this->radius) {
			this->blackHole = true;
		}
		else {
			this->blackHole = false;
		}
	}

	void setTime(double time){
        this->time = time;
	}

    //updating
    void updatePosition(){
        time = time+dt;
        position = position+velocity.scale(dt);
    }

    void updateVelocity(double dt,double v_mag_ship){
        double v_mag=velocity.getMagnitude();
        double dv=v_mag_ship-v_mag;
        if(dv>0){
            dv=dv/(1-v_mag_ship*v_mag/(C*C));
            this->dt=dt/sqrt(1-dv*dv/(C*C));
        }
        else{
            dv=dv/(1-v_mag_ship*v_mag/(C*C));
            this->dt=dt*sqrt(1-dv*dv/(C*C));
        }
        velocity=velocity+acceleration.scale(this->dt);
    }

    void updateAcceleration(Vector3 intensity){
        acceleration=intensity;
    }
	void updateTemperature(double temperature) {
		this->temperature = temperature;

	}

    //Getting values
    double getMass(){
        return mass;
    }
	double getRadius() {
		return radius;
	}
	double getEmissivity() {
		return this->emissivity;
	}
	double getTemperature() {
		return temperature;
	}
	bool isBlackBody() {
		return blackBody;
	}
    Vector3 getPosition(){
        return position;
    }
    double getTime(){
        return time;
    }
	bool isBlackHole() {
		return this->blackHole;
	}
    Vector3 getVelocity(){
        return velocity;
    }

	double calculateTemperature(Vector3 effectPosition) {
		double tPower4 = 0;

		double a = (1 - (velocity.getValue1()*velocity.getValue1() + velocity.getValue2()*velocity.getValue2() +
			velocity.getValue3()*velocity.getValue3()) / (C*C));
		double b = -2.0*(time - (velocity.getValue1()*effectPosition.getValue1() + velocity.getValue2()*effectPosition.getValue2() +
			velocity.getValue3()*effectPosition.getValue3()) / (C*C));
		double c = (time*time - (effectPosition.getValue1()*effectPosition.getValue1() + effectPosition.getValue2()*effectPosition.getValue2()
			+ effectPosition.getValue3()*effectPosition.getValue3()) / (C*C));

		double temp = sqrt(b*b - 4.0*a*c);

		if (temp < 0.0) {
			return tPower4;
		}
		double t = (-b - temp) / (2.0*a);

		if (t < 0.0) {
			t = (-b + temp) / (2.0*a);
		}
		if (t > time) {
			return tPower4;
		}

		double dt = time - t;

		Vector3 causePosition = position - Vector3::getVector(velocity.getValue1()*dt, velocity.getValue2()*dt, velocity.getValue3()*dt);
		Vector3 dr = effectPosition - causePosition;
		double drMag = dr.getMagnitude();
		//cout << temperature << endl;
		tPower4 = emissivity * (radius*radius*temperature*temperature*temperature*temperature) / (4 * drMag*drMag);
		//cout << tPower4 << endl;

		return tPower4;
	}
    Vector3 getGravitationalField(Vector3 effectPosition){
        Vector3 intensity;

        double a = (1-(velocity.getValue1()*velocity.getValue1()+velocity.getValue2()*velocity.getValue2()+
                       velocity.getValue3()*velocity.getValue3())/(C*C));
		double b = -2.0*(time-(velocity.getValue1()*effectPosition.getValue1()+velocity.getValue2()*effectPosition.getValue2()+
                         velocity.getValue3()*effectPosition.getValue3())/(C*C));
		double c = (time*time-(effectPosition.getValue1()*effectPosition.getValue1()+effectPosition.getValue2()*effectPosition.getValue2()
				+effectPosition.getValue3()*effectPosition.getValue3())/(C*C));

        double temp=sqrt(b*b-4.0*a*c);

		if(temp<0.0) {
			return intensity;
		}
		double t =  (-b-temp)/(2.0*a);

		if(t<0.0) {
			t=(-b+temp)/(2.0*a);
		}
		if(t>time) {
			return intensity;
		}

		double dt = time-t;

        Vector3 causePosition = position-Vector3::getVector(velocity.getValue1()*dt,velocity.getValue2()*dt,velocity.getValue3()*dt);
        Vector3 dr = effectPosition-causePosition;
        double GM = G*mass;
        double r_mag=dr.getMagnitude();

        double L2=GM*r_mag/(1-3*GM/(C*C*r_mag));

        intensity = dr.scale(-(GM/(r_mag*r_mag*r_mag)/*-L2/(r_mag*r_mag*r_mag*r_mag)*/+3*GM*L2/(C*C*r_mag*r_mag*r_mag*r_mag*r_mag)));

        return intensity;
    }


};
