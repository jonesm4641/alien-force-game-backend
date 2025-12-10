Alien Force Multiplayer Game – Backend System
This repository contains the backend architecture and source code for the Alien Force multiplayer game project. The system is implemented in C++ and includes modular components for player logic, enemy behavior, projectile management, and database support. The backend is designed with object-oriented principles to support scalability, clarity, and long-term maintainability.
________________________________________
Project Overview
The backend provides core logic for a multiplayer gameplay environment, including:
•	Player state and behavior management
•	Enemy movement and interaction handling
•	Projectile creation, movement, and collision processing
•	Foundational database structure for storing and retrieving game data
•	Backend processing to support client-side game features
This repository represents the backend development portion of the larger game project.
________________________________________
Repository Structure
alien-force-game-backend/
│
├── AlienForceClient.cpp        // Backend-client interaction logic
├── AlienForceClient.vcxproj    // Visual Studio project configuration
│
├── Db.hpp                      // Database interface definitions
├── Db.cpp                      // Database operation implementations
├── db.cfg                      // Database configuration file
│
├── Player.hpp                  // Player class declarations
├── Player.cpp                  // Player class implementation
│
├── Enemy.hpp                   // Enemy class declarations
├── Enemy.cpp                   // Enemy class implementation
│
├── Projectile.hpp              // Projectile class declarations
├── Projectile.cpp              // Projectile class implementation
│
├── CheckStubs.cpp              // Test utilities and stub functions
________________________________________
Key Components
Player Module
Handles player attributes, actions, and gameplay logic.
Defined in Player.hpp and Player.cpp.
Enemy Module
Implements enemy behavior, tracking, and interactions.
Located in Enemy.hpp and Enemy.cpp.
Projectile Module
Manages projectile lifecycle, including creation, trajectory updates, and collision handling.
Implemented in Projectile.hpp and Projectile.cpp.
Database Layer
Provides a structural foundation for database interactions.
Includes Db.hpp, Db.cpp, and db.cfg.
Client Logic
AlienForceClient.cpp acts as the backend execution point used for testing and integrating gameplay components.
________________________________________
Current Development Status
The database integration layer is included in the project; however, a full runtime connection between the game backend and the database has not yet been implemented.
The Db.cpp, Db.hpp, and db.cfg files define the intended architecture, but establishing a working database connection remains a planned enhancement for future development.
________________________________________
Build and Execution
Requirements
•	A C++17-compatible compiler (Microsoft Visual Studio recommended)
•	MySQL connector or database library if database functionality is extended
•	Proper configuration in db.cfg if database integration is pursued
Build Instructions (Visual Studio)
1.	Open AlienForceClient.vcxproj
2.	Select Debug or Release
3.	Build and run the project
Build Instructions (Command Line)
g++ AlienForceClient.cpp Db.cpp Player.cpp Enemy.cpp Projectile.cpp -o AlienForceGame
Adjust include paths and library flags as needed.
________________________________________
Planned Enhancements
•	Complete database connectivity and testing
•	Expanded enemy AI behaviors
•	Additional projectile and weapon systems
•	Networking features for real multiplayer interaction
•	Data persistence for player stats and game states
•	Integration with a graphics or rendering engine
________________________________________
Author
Marc-Anthony Jones
Cybersecurity & IT Systems Professional
LinkedIn: www.linkedin.com/in/marc-anthony-jones-5b359122b

