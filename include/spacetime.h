/**
    according to frame of reference of ship
**/
#include "matter.h"
#include <map>

#define N 3

class Spacetime{
private:
    double grid[N][N][N][3];
    map <int,Matter> matters;
	double dt = 60.0*60.0/30.0;//this second per frame
	double max_acceleration = 6000000.0;
    double speedLimits[4] = {16000.00, 90000.00, 0.25*C, C};
    double accelerations[4] = {160.00/dt, 900.00/dt, 0.0025*C/dt, 0.01*C/dt};
    double gravDilFactor;

    //Property of spaceship
    double time;
    Vector3 position;
    Vector3 velocity;

    Vector3 posFromMatter;
    bool lock;
    int lockId;
    int currentSpaceshipType;
public:
    Spacetime(){
        lock = false;
        time = 0;
        lockId = 3;
        position.setValue(145.5e9, 0, 0);
        currentSpaceshipType = 1;
        gravDilFactor=1;
    }

    /**Toggle Lock**/
    void toggleLock(){
        lock=!lock;
        this->velocity = Vector3::getVector(0,0,0);
        posFromMatter = position - getMatter(0).getPosition();
        double temp=posFromMatter.getMagnitude();
        lockId = 0;
        for (auto& m: matters){
            posFromMatter = position - m.second.getPosition();
            if((temp-posFromMatter.getMagnitude())>=2e6){
                temp = posFromMatter.getMagnitude();
                lockId = m.first;
            }
        }
        posFromMatter = position-getMatter(lockId).getPosition();
    }

    /**Position is relative to physics engine*/
    void setPosition(glm::vec3 position, double scale){
        this->position.setValue(double(position.x)/scale, double(position.y)/scale, double(position.z)/scale);
    }

    void setSpaceShipType(int type){
        this->velocity = Vector3::getVector(0.0, 0.0, 0.0);
        currentSpaceshipType = type;
    }

    /**Velocity is relative to physics engine*/
    void accelerate(glm::dvec3 acceleration){
        double magnetude_of_acceleration = accelerations[currentSpaceshipType]*(7.2/(6.0+30.0*velocity.getMagnitude()/speedLimits[currentSpaceshipType])-0.2);
        Vector3 temp_velocity = this->velocity + Vector3::getVector(acceleration.x, acceleration.y, acceleration.z).scale(dt*magnetude_of_acceleration);
        if(temp_velocity.getMagnitude()<speedLimits[currentSpaceshipType]){this->velocity = temp_velocity;}
    }

    void forceShipToStop(){
        this->velocity = this->velocity.scale(0);
    }

    void forceSynchronize(){
        for (auto& m: matters) {
            m.second.setTime(this->time);
        }
    }

    void addMatter(int id,Matter matter){
        matters[id]=matter;
    }

    void update(){

        time=time+dt;
        Vector3 intensity;

        if(!lock){
//            for (auto& m: matters){
//                intensity =intensity + m.second.getGravitationalField(position).scale(9e28/m.second.getMass());
//            }
            intensity = getGravitationalField(position);
            //Update the velocity and position from space time gravity
            this->velocity = this->velocity+intensity.scale(dt);
            this->position = this->position+this->velocity.scale(dt);
        }else{
            posFromMatter = posFromMatter+this->velocity.scale(dt);
            position = getMatter(lockId).getPosition() + posFromMatter;
        }
		double temperatureNew;
		double tPower4 = 0.0;
		double radius;
		double emissivity;
        double v_mag_ship=velocity.getMagnitude();

        if(lock){
            Vector3 temp = matters.at(lockId).getVelocity()+this->velocity;
            temp.scale(1.0+abs(velocity.dot(matters.at(lockId).getVelocity()))/(C*C));
            v_mag_ship = temp.getMagnitude();
        }

        //acceleration update
        for (auto& m: matters){
            intensity = this->getGravitationalField(m.second.getPosition(),m.first);
            m.second.updateAcceleration(intensity);
        }

        gravDilFactor=0;
        for (auto& m: matters) {
            double a = (1-(velocity.getValue1()*velocity.getValue1()+velocity.getValue2()*velocity.getValue2()+
                velocity.getValue3()*velocity.getValue3())/(C*C));
            double b = -2.0*(time-(velocity.getValue1()*position.getValue1()+velocity.getValue2()*position.getValue2()+
                velocity.getValue3()*position.getValue3())/(C*C));
            double c = (time*time-(position.getValue1()*position.getValue1()+position.getValue2()*position.getValue2()
				+position.getValue3()*position.getValue3())/(C*C));

            double temp=sqrt(b*b-4.0*a*c);

            if(temp<0.0) {
                continue;
            }
            double t =  (-b-temp)/(2.0*a);

            if(t<0.0) {
                t=(-b+temp)/(2.0*a);
            }
            if(t>time) {
                continue;
            }

            double dt = time-t;

            Vector3 causePosition = m.second.getPosition()-Vector3::getVector(velocity.getValue1()*dt,velocity.getValue2()*dt,velocity.getValue3()*dt);
            Vector3 dr = position-causePosition;
            double GM = G*m.second.getMass();
            double r_mag=dr.getMagnitude();

            gravDilFactor+=GM/r_mag;
        }

        for (auto& m: matters) {
            double a = (1-(velocity.getValue1()*velocity.getValue1()+velocity.getValue2()*velocity.getValue2()+
                velocity.getValue3()*velocity.getValue3())/(C*C));
            double b = -2.0*(time-(velocity.getValue1()*position.getValue1()+velocity.getValue2()*position.getValue2()+
                velocity.getValue3()*position.getValue3())/(C*C));
            double c = (time*time-(position.getValue1()*position.getValue1()+position.getValue2()*position.getValue2()
				+position.getValue3()*position.getValue3())/(C*C));

            double temp=sqrt(b*b-4.0*a*c);

            if(temp<0.0) {
                continue;
            }
            double t =  (-b-temp)/(2.0*a);

            if(t<0.0) {
                t=(-b+temp)/(2.0*a);
            }
            if(t>time) {
                continue;
            }

            double dt = time-t;

            Vector3 causePosition = m.second.getPosition()-Vector3::getVector(velocity.getValue1()*dt,velocity.getValue2()*dt,velocity.getValue3()*dt);
            Vector3 dr = position-causePosition;
            double GM = G*m.second.getMass();
            double r_mag=dr.getMagnitude();

            double netGravDilFactor = gravDilFactor-GM/r_mag;
            netGravDilFactor=1/sqrt(1-2*netGravDilFactor/(C*C));
            m.second.updateVelocity(this->dt*netGravDilFactor,v_mag_ship);
        }

        //position update
        for (auto& m: matters) {
            m.second.updatePosition();
        }
		for (auto& m : matters){
			if (!m.second.isBlackBody()) {
				tPower4 = this->calculateTemperature(m.second.getPosition(), m.first);
				emissivity = m.second.getEmissivity();
				temperatureNew = sqrt(sqrt(emissivity*tPower4));
				m.second.updateTemperature(temperatureNew);
			}
		}
    }

    /**This function expects user to pass the center of grid*/
    void gridUpdate(double init_x,double init_y,double init_z,double gridLength){
        double scale=1,x,y,z;
        init_x -= N*gridLength/2.0;
        init_y -= N*gridLength/2.0;
        init_z -= N*gridLength/2.0;
        Vector3 intensity;
        for(int i=0;i<N;i++){
            for(int j=0;j<N;j++){
                for(int k=0;k<N;k++){
                    x=init_x+i*gridLength;
                    y=init_y+j*gridLength;
                    z=init_z+k*gridLength;
                    intensity = getGravitationalField(Vector3::getVector(x,y,z));
                    x+=scale*intensity.getValue1();
                    y+=scale*intensity.getValue2();
                    z+=scale*intensity.getValue3();
                    grid[i][j][k][0]=x;
                    grid[i][j][k][1]=y;
                    grid[i][j][k][2]=z;
                }
            }
        }
    }

    //Getting
	double calculateTemperature(Vector3 effectPosition, int id = -1) {
		double tPower4 = 0;
		for (auto& m : matters) {
			if (id != m.first) {
				tPower4 = tPower4 + m.second.calculateTemperature(effectPosition);
			}
		}
		return tPower4;
	}

    Vector3 getGravitationalField(Vector3 effectPosition,int id=-1){
        Vector3 intensity;
        gravDilFactor=0;
        for (auto& m: matters) {
            if(id!=m.first){
                intensity=intensity+m.second.getGravitationalField(effectPosition);
            }
        }
        return intensity;
    }

    Matter getMatter(int id){
        Matter m = matters.at(id);
        return m;
    }

    /**
    *Returns the time in space ship time reference
    */
    double getTime(){
        return time;
    }

    glm::vec3 getPosition(double scale){
        return glm::vec3(scale*this->position.getValue1(), scale*this->position.getValue2(), scale*this->position.getValue3());
    }

    glm::vec3 getDirection(){
        glm::vec3 dir = glm::vec3(glm::normalize(glm::dvec3(velocity.getValue1(), velocity.getValue2(), velocity.getValue3())));
        return dir;
    }

    double getVelRatioWithC(){
        return velocity.getMagnitude()/3.0e8;
    }

    double getVelocity(){
        return velocity.getMagnitude();
    }

    int getSpaceShipType(){
        return currentSpaceshipType;
    }

    double getDt(){
        return dt;
    }

    bool isLocked(){
        return lock;
    }

    int getLockedId(){
        return lockId;
    }

    void getGrid(double grid[N][N][N][3]){
         for(int i=0;i<N;i++){
            for(int j=0;j<N;j++){
                for(int k=0;k<N;k++){
                    grid[i][j][k][0]=this->grid[i][j][k][0];
                    grid[i][j][k][1]=this->grid[i][j][k][1];
                    grid[i][j][k][2]=this->grid[i][j][k][2];
                }
            }
        }
    }
};
