# Resource Management System
The premise of this system is fairly straight forward, right now the most daunting thing is refactoring the GLTF loading system to integrate with this new system.

## Brokers / Provider (which name sounds cooler?)
This classification is what is typically defined to hold specific data, for example the GLTF provider would be responsible for loading GLTF files and providing the data to the system, the Texture provider would be responsible for loading textures and providing them to the system, etc.

## Resource / Handle / Consumer
This is a wrapped class around the data that is provided by the brokers, this is how you use the data itself, the data should not be managed or manipulated from within the broker itself.
