#### Creates
- A [Pipeline object](Pipeline), choosing the default config and the Shader files

#### Manages
- The data sent to the Shaders (push constants, [descriptor sets](Descriptors))
- Defines a Pipeline layout used to create its Pipeline
- Render specific objects using said Pipeline object

#### Info
An [App](App) might want to use different shaders for different types of objects, to do so it will use several Render Systems each with their own Pipeline. Each Render System will be specialized in rendering one type of object

#RenderEngine