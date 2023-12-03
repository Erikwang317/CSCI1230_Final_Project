## Project 6: Action!

Please fill this out for Action! only. The project handout can be found [here](https://cs1230.graphics/projects/realtime/2).

### Output Comparison

> [!IMPORTANT]
> Before generating expected outputs, make sure to:
>
> 1. Set your working directory to the project directory
> 2. From the project directory, run `git submodule update --recursive --remote` to update the `scenefiles` submodule.
> 3. Change all instances of `"lights-camera"` in `mainwindow.cpp` to `"action"` (there should be 2 instances, one in `MainWindow::onUploadFile` and one in `MainWindow::onSaveImage`).

Run the program, open the specified `.json` file and follow the instructions to set the parameters.

If the output you are generating is an image, save the image with the specified file name using the "Save image" button in the UI. Be sure to follow the instructions in the left column to set the file name. Once you save the images, they will appear in the table below.

If the output you are generating is a video, reference the [Screen Recording Guide](https://cs1230.graphics/docs/screen-recording) to capture a video of the specified interaction. Once you've recorded everything, navigate to this template file in Github, click edit, and either select or drag-and-drop each of your videos into the correct location. This will upload them to GitHub (but not include them in the repo) and automatically embed them into this Markdown file by providing a link. Make sure to double-check that they all show up properly in the preview.

We're **not** looking for your video to **exactly** match the expected output (i.e. draw the exact same shape). Just make sure to follow the instructions and verify that the recording fully demonstrates the functionality of that part of your project.

> [!NOTE]
> There may be issues with the image saving functionality, and the images may not save the exact same way as the expected outputs. This is acceptable, as long as your output demonstrates correct functionality.

#### Point and Spot Lights

|                                File/Method To Produce Output                                 |                                                Expected Output                                                 |                                                       Your Output                                                       |
| :------------------------------------------------------------------------------------------: | :------------------------------------------------------------------------------------------------------------: | :---------------------------------------------------------------------------------------------------------------------: |
| Input: `point_light_1.json`<br/>Output: `point_light_1.png`<br/>Parameters: (5, 5, 0.1, 100) | ![](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/action/required_outputs/point_light_1.png) | ![Place point_light_1.png in student_outputs/action/required folder](student_outputs/action/required/point_light_1.png) |
| Input: `point_light_2.json`<br/>Output: `point_light_2.png`<br/>Parameters: (5, 5, 0.1, 100) | ![](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/action/required_outputs/point_light_2.png) | ![Place point_light_2.png in student_outputs/action/required folder](student_outputs/action/required/point_light_2.png) |
| Input: `spot_light_1.json`<br/>Output: `spot_light_1.png`<br/> Parameters: (5, 5, 0.1, 100)  | ![](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/action/required_outputs/spot_light_1.png)  |  ![Place spot_light_1.png in student_outputs/action/required folder](student_outputs/action/required/spot_light_1.png)  |
|  Input: `spot_light_2.json`<br/>Output: `spot_light_2.png`<br/>Parameters: (5, 5, 0.1, 100)  | ![](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/action/required_outputs/spot_light_2.png)  |  ![Place spot_light_2.png in student_outputs/action/required folder](student_outputs/action/required/spot_light_2.png)  |

#### Invert

> [!NOTE]
> If you did not implement the invert filter, you can skip this section.

|                                                      File/Method To Produce Output                                                      |                                                      Expected Output                                                      |                                                                  Your Output                                                                  |
| :-------------------------------------------------------------------------------------------------------------------------------------: | :-----------------------------------------------------------------------------------------------------------------------: | :-------------------------------------------------------------------------------------------------------------------------------------------: |
| Input: `primitive_salad_1.json`<br/>**Apply invert filter**<br/>Output: `primitive_salad_1_invert.png`<br/>Parameters: (5, 5, 0.1, 100) | ![](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/action/required_outputs/primitive_salad_1_invert.png) | ![Place primitive_salad_1_invert.png in student_outputs/action/required folder](student_outputs/action/required/primitive_salad_1_invert.png) |

#### Grayscale

> [!NOTE]
> If you did not implement the grayscale filter, you can skip this section.

|                                                         File/Method To Produce Output                                                         |                                                       Expected Output                                                        |                                                                     Your Output                                                                     |
| :-------------------------------------------------------------------------------------------------------------------------------------------: | :--------------------------------------------------------------------------------------------------------------------------: | :-------------------------------------------------------------------------------------------------------------------------------------------------: |
| Input: `primitive_salad_1.json`<br/>**Apply grayscale filter**<br/>Output: `primitive_salad_1_grayscale.png`<br/>Parameters: (5, 5, 0.1, 100) | ![](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/action/required_outputs/primitive_salad_1_grayscale.png) | ![Place primitive_salad_1_grayscale.png in student_outputs/action/required folder](student_outputs/action/required/primitive_salad_1_grayscale.png) |

> [!WARNING]
> Kernel-based filters might not work the same on different devices. This is explained in more detail in the project handout. If your output doesn't seem to quite match our output but you think your implementation is correct, be sure to let your mentor know.

#### Sharpen

> [!NOTE]
> If you did not implement the sharpen filter, you can skip this section.

|                                                        File/Method To Produce Output                                                        |                                                       Expected Output                                                       |                                                                    Your Output                                                                    |
| :-----------------------------------------------------------------------------------------------------------------------------------------: | :-------------------------------------------------------------------------------------------------------------------------: | :-----------------------------------------------------------------------------------------------------------------------------------------------: |
| Input: `recursive_sphere_4.json`<br/>**Apply sharpen filter**<br/>Output: `recursive_sphere_4_sharpen.png`<br/>Parameters: (5, 5, 0.1, 100) | ![](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/action/required_outputs/recursive_sphere_4_sharpen.png) | ![Place recursive_sphere_4_sharpen.png in student_outputs/action/required folder](student_outputs/action/required/recursive_sphere_4_sharpen.png) |

#### Blur

> [!NOTE]
> If you did not implement the blur filter, you can skip this section.

|                                                     File/Method To Produce Output                                                     |                                                     Expected Output                                                      |                                                                 Your Output                                                                 |
| :-----------------------------------------------------------------------------------------------------------------------------------: | :----------------------------------------------------------------------------------------------------------------------: | :-----------------------------------------------------------------------------------------------------------------------------------------: |
| Input: `recursive_sphere_4.json`<br/>**Apply blur filter**<br/>Output: `recursive_sphere_4_blur.png`<br/>Parameters: (5, 5, 0.1, 100) | ![](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/action/required_outputs/recursive_sphere_4_blur.png) | ![Place recursive_sphere_4_blur.png in student_outputs/action/required folder](student_outputs/action/required/recursive_sphere_4_blur.png) |

#### Camera Translation

_Instructions:_ Load `chess.json`. For about 1 second each in this order, press:

- <kbd>W</kbd>, <kbd>A</kbd>, <kbd>S</kbd>, <kbd>D</kbd> to move in each direction by itself
- <kbd>W+A</kbd> to move diagonally forward and to the left
- <kbd>S+D</kbd> to move diagonally backward and to the right
- <kbd>Space</kbd> to move up
- <kbd>Cmd/Ctrl</kbd> to move down

##### Expected Output

https://github.com/BrownCSCI1230/projects_realtime_template/assets/45575415/710ff8b4-6db4-445b-811d-f6c838741e67

##### Your Output

<!---
Paste your output on top of this comment!
-->

#### Camera Rotation

_Instructions:_ Load `chess.json`. Take a look around!

##### Expected Output

https://github.com/BrownCSCI1230/projects_realtime_template/assets/45575415/a14f4d32-88ee-4f5f-9843-74dd5c89b9dd

##### Your Output

<!---
Paste your output on top of this comment!
-->

### Design Choices
*My code is run on macOS. The default FBO is set to 2.*

---
#### 2.1 Lights
The implemention for phong is equivalent to the previous raytracing projects. Lights information are passed to the shader program as uniforms when ```Realtime::paintExampleGeometry()``` is called. In particular, we compute the phong for each pixel inside the fragment shader, ie, ```default.frag```. 

---
#### 2.2 Camera Movement
The interactive camera UI is implemented in ```Realtime::timerEvent(QTimerEvent *event)``` and  ```Realtime::mouseMoveEvent(QMouseEvent *event)```, where each key's activation corresponds to a ```Camera::moveCamera____()``` function.

For ```W, S, A, D, SPACE, CTRL```, the position field, and subsequently, the view and projection matrix, for ```m_camera``` are updated. 

For mouse movement, the look field, and subsequently, the view and projection matrix, for ```m_camera``` are updated. Given a normalized axis vector, the look vector is transformed by a 3*3 matrix computed using the Rodrigues' formula.


---
#### 2.3 FBO, and blur kernel convolution
The high level logic is as follows. We use 2 shader program. The first shader program stores the rendered scene (scene shapes data + phong) inside ```m_fbo```. The second program renders the final window, ```m_defaultFBO```, as a texture mapping process by mapping uv values from ```m_fbo_texture``` to the screen. For texture mapping, we need to create a pair of ```m_fullscreen_vbo``` & ```m_fullscreen_vao``` that draws 6 vertices and 2 triangles representing fragments of the full screen. Convolution is done inside the fragment shader ```texture.frag```. We use a sampler to sample from neighbor pixels *unit texel size* away. A unit texel size is pre-computed and passed to the shader program as a uniform, which has two components that translates a pixel size displacement from screen to uv map, *1/screen_width* & *1/screen_height*. 

My implementation for FBO's is *almost* identical to lab11, where FBO's are initialized in the end of ```Realtime::initializeGL()``` via ```Realtime::makeFBO()```. ```Realtime::paintGL()``` also consists of two parts. ```Realtime::paintExampleGeometry()``` draws our scene to the off-screen frame ```m_fbo```. Lastly, ```Realtime::paintTexture()``` does texture-mapping from ```m_fbo_texture``` to the default frame.

---
### Collaboration/References
NA

### Known Bugs
NA

### Extra Credit
NA
