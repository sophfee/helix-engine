#pragma once

class Engine {
private:
	static Engine* instance;
	
public:
	
	static Engine* get_instance();
};
