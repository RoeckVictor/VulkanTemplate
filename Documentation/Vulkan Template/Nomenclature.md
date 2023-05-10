- Queue Family : Almost every operation in Vulkan, anything from drawing to uploading textures, requires commands to be submitted to a queue. There are different types of queues that originate from different _queue families_ and each family of queues allows only a subset of commands. For example, there could be a queue family that only allows processing of compute commands or one that only allows memory transfer related commands.

- Validation Layer : Validation layers are optional components that hook into Vulkan function calls to apply additional operations, used for error/warning management

- Physical Device : Represents the actual, physical GPU used to do the computations 

- Logical Device : The logical device is a handle to interface with the physical device, we use it to execute functions, send and receive information from the GPU. There can be several logical devices, each with their own state and resources independent of other logical devices

- Extension : 

- Presentation mode : The presentation mode represents the algorithm that checks the conditions for "swapping" images to the screen

- Swap Chain :

- Instance :

- Host :

- Render Target :

- Render Pass :

- Frame Buffer : 

- Command Pool :

- Subpass : 

- Staging Buffer :

- Descriptor Set : 

- Descriptor Set Layout :

- Descriptor Pool :

- Memory Barrier : 