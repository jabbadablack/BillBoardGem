# Billboard Gem for O3DE

Hello! This is a simple, lightweight Gem for O3DE that adds a custom **Billboard Component**. 

I made this to help with rendering 2D sprites, grass, and background elements in a 3D world. Instead of writing complex custom shaders, you can just drop this component onto any standard mesh, and it will automatically handle the math to make it face the camera.

## Features

* **Camera Tracking:** The mesh will smoothly track the active camera and stay perfectly parallel to the screen.
* **Angle Offsets:** A built-in rotation offset so you can easily create intersecting "cross-quad" meshes for things like grass and trees.

## How to Use It

1. Enable the **Billboard** in your O3DE Project Manager.
2. Build your project.
3. Open the Editor and create a new Entity.
4. Add a standard **Mesh Component**, give it a model (like a plane) and a **Material Component** for your texture.
5. Add the **Billboard Component**.
6. Click the target icon next to "Target Entity" and select your scene's Camera.

That's it! When you press Play, your mesh will snap to face the camera.

## Component Settings

* **Enable Billboard:** Check this to turn the tracking on or off.
* **Target Entity:** The camera (or any object) you want the billboard to look at.
* **Forward Axis:** Choose which side of your 3D model is the "front" (defaults to Y-Negative, which works best for standard O3DE cameras).
* **Angle Offset:** Type in a degree (like 0, 45, or 90) to twist the mesh.