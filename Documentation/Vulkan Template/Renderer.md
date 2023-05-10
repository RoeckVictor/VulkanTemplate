#### Creates
- The **Command Buffers** (one for each frame in flight)

#### Manages
- Frame status (index, started)
- Begin and End the **Render Passes**
- [Swap Chain](SwapChain) re-creation
- Current Command Buffer
- Detects if the [window](Window) was resized

#RenderEngine