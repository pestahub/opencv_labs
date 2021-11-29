#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>


class Body {
public:
	int id;
	float mass;
	cv::Scalar color;

	cv::Point2d currentPos;
	cv::Vec2d velocity;
	cv::Vec2d acceleration;
	cv::Vec2d force;
	
	Body(int id, float mass, cv::Scalar color, cv::Point origin);

	void updateKin(cv::Vec2d vel, cv::Vec2d acc, cv::Vec2d force);
	void updateCoords();
	void Body::invertYVel();
	void Body::invertXVel();

	double operator ^(const Body& another);
	bool operator ==(const Body& another);
};

Body::Body(int id, float mass, cv::Scalar color, cv::Point origin)
{
	this->id = id;
	this->mass = mass;
	this->color = color;
	this->currentPos = origin;
}

void Body::updateKin(cv::Vec2d vel, cv::Vec2d acc, cv::Vec2d force)
{
	this->velocity = this->velocity + vel;
	this->force = force;
	this->acceleration = acc;
}

void Body::updateCoords()
{
	cv::Point2d oldPos = this->currentPos;
	oldPos.x = oldPos.x + this->velocity[0];
	oldPos.y = oldPos.y + this->velocity[1];
	this->currentPos = oldPos;

	//std::cout << oldPos << " | " << currentPos << " | " << this->velocity << std::endl;
}

void Body::invertXVel()
{
	velocity[0] = -velocity[0];
}

void Body::invertYVel()
{
	velocity[1] = -velocity[1];
}



// overloading xor to calculate distance
double Body::operator ^(const Body& another) {
	double deltaX = another.currentPos.x - this->currentPos.x;
	double deltaY = another.currentPos.y - this->currentPos.y;
	return sqrt(deltaX * deltaX + deltaY * deltaY);
}

bool Body::operator ==(const Body& another)
{
	return this->id == another.id;
}

class World {
public:
	std::vector<Body> bodies;
	int tick;
	double dt;
	double gravCoef;
	cv::Mat background;

	int freeId;

	World(double dt, double gravCoef, cv::Mat background);
	void addBody(float mass, cv::Scalar color, cv::Point origin);
	void setVelocity(int bodiId, cv::Vec2d velcoty);


	double calcForce2Bodies(Body first, Body second);
	cv::Vec2d calcForcesOfBody(Body targetBody);
	void calculateNextStep();
	void drawFrame();
	int run();
};

World::World(double dt, double gravCoef, cv::Mat background)
{
	tick = 0;
	freeId = 0;
	this->dt = dt;
	this->gravCoef = gravCoef;
	this->background = background;

	cv::namedWindow("Multibody");


}

void World::addBody(float mass, cv::Scalar color, cv::Point origin)
{
	Body newBody(freeId, mass, color, origin);
	freeId++;

	bodies.push_back(newBody);
}

void World::setVelocity(int bodyId, cv::Vec2d velocity)
{
	bodies[bodyId].velocity = velocity;
}

double World::calcForce2Bodies(Body first, Body second)
{
	double distance = (first ^ second);
	//std::cout << "dist: " << distance << std::endl;
	if (distance < 50) return 0;
	return gravCoef * first.mass * second.mass / (distance * distance);
}

cv::Vec2d World::calcForcesOfBody(Body targetBody)
{
	cv::Vec2d effect(0,0);
	for (int id = 0; id < freeId; id++)
	{
		Body& entity = bodies[id];
		if (entity == targetBody) continue;

		double deltaX = entity.currentPos.x - targetBody.currentPos.x;
		double deltaY = entity.currentPos.y - targetBody.currentPos.y;
		double angle = atan2(deltaY, deltaX);
		double sina = sin(angle);
		double cosa = cos(angle);

		double absForce = calcForce2Bodies(targetBody, entity);
		cv::Vec2d force(absForce * cosa, absForce * sina);
		effect += force;

		//std::cout << hypoten << " | " << sina << std::endl;
	}
	return effect;
}

void World::calculateNextStep()
{
	cv::Vec2d resForce(0,0);
	for (int id = 0; id < freeId; id++)
	{
		Body& entity = bodies[id];

		cv::Vec2d effect = calcForcesOfBody(entity);

		cv::Vec2d acc = effect / entity.mass;

		entity.updateKin(acc * dt, acc, effect);
		//std::cout << "After: " << bodies[0].id << " | " << bodies[0].velocity << std::endl;

		resForce += effect;

		//std::cout <<  "Vc | Acc: " << acc * dt << " | " << acc << std::endl;
	}
	//std::cout << "resForce: " << resForce << std::endl;
	for (int id = 0; id < freeId; id++)
	{
		bodies[id].updateCoords();
		cv::Point2d& realPos = bodies[id].currentPos;
		cv::Point2d fixedPos = realPos;
		bool invertX = false;
		bool invertY = false;
		
		if (realPos.x < 0) { fixedPos.x = -realPos.x; invertX = true; }
		if (realPos.x > background.cols) { fixedPos.x = 2 * background.cols - realPos.x; invertX = true; }
		if (realPos.y < 0) { fixedPos.y = -realPos.y; invertY = true; }
		if (realPos.y > background.rows) { fixedPos.y = 2 * background.rows - realPos.y; invertY = true; }
		bodies[id].currentPos = fixedPos;
		if (invertX) bodies[id].invertXVel();
		if (invertY) bodies[id].invertYVel();
		
		//std::cout <<  id << " | " << fixedPos << std::endl;
	}

	tick++;
	// Find center of the masses?
}

void World::drawFrame()
{
	cv::Mat frame(background.rows, background.cols, background.type());
	for each (Body body in bodies)
	{
		cv::circle(background, body.currentPos, 1, body.color);
	}

	background.copyTo(frame);
	
	for each (Body body in bodies)
	{
		cv::circle(frame, body.currentPos, 10, body.color);
	}

	cv::imshow("Multibody", frame);
}

int World::run()
{
	std::cout << "Starting sim" << std::endl;
	char key = ' ';
	do
	{
		calculateNextStep();
		drawFrame();
		std::cout << this->tick << std::endl;
		key = (char)cv::waitKey(13);
	} while (key != 'e');
	return 1;
}


int main() {

	World simulation(241, 6.6*pow(10,-11), cv::Mat(1080, 1080, CV_8UC3));
	simulation.addBody(pow(10, 9) * 3, cv::Scalar(240, 0, 0), cv::Point(300, 500));	// землюшка
	simulation.setVelocity(0, cv::Vec2d(0, 1.5));

	simulation.addBody(pow(10, 9) * 10, cv::Scalar(0, 240, 255), cv::Point(450, 500)); // солныщко
	simulation.bodies[1].velocity = cv::Vec2d(2, 0);

	simulation.addBody(pow(10, 9) / 2, cv::Scalar(0, 0, 240), cv::Point(600, 500));
	simulation.bodies[2].velocity = cv::Vec2d(0, -2);
	simulation.run();

	char key = (char)cv::waitKey(-1);
}