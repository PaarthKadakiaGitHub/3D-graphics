#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include"shader_m.h"

#include <iostream>
#include <cmath>



static unsigned int ss_id = 0;
//sun
float get_sun_rotate_angle_around_itself(float day)
{
    float onedayrotation = 360.0f / 27.0f;
    return day * onedayrotation;
}

//earth
float get_earth_rotate_angle_around_sun(float day) {
    float onedayrotation = 360.0f / 365.0f;
    return day * onedayrotation;
}
float get_earth_rotate_angle_around_itself(float day) {
    float onedayrotation = 360.0f / 1.0f;
    return day * onedayrotation;
}
//moon
float get_moon_rotate_angle_around_earth(float day) {
    float onedayrotation = 360.0f / 27.0f;
    return day * onedayrotation;
}
float get_moon_rotate_angle_around_itself(float day) {
    float onedayrotation = 360.f / 27.0f;
    return day * onedayrotation;

}
void dump_framebuffer_to_ppm(std::string prefix, unsigned int width, unsigned int height) {
    int pixelChannel = 3;
    int totalPixelSize = pixelChannel * width * height * sizeof(GLubyte);
    GLubyte* pixels = new GLubyte[totalPixelSize];
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    std::string file_name = prefix + std::to_string(ss_id) + ".ppm";
    std::ofstream fout(file_name);
    fout << "P3\n" << width << " " << height << "\n" << 255 << std::endl;
    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            size_t cur = pixelChannel * ((height - i - 1) * width + j);
            fout << (int)pixels[cur] << " " << (int)pixels[cur + 1] << " " << (int)pixels[cur + 2] << " ";
        }
        fout << std::endl;
    }
    ss_id++;
    delete[] pixels;
    fout.flush();
    fout.close();
}

int normalize = 100000;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;
// camera
glm::vec3 cameraPos = glm::vec3(100.0f, 50.0f, 100.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("default.vert", "default.frag");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions         // colors

    -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 0.0f, //back face
     1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
     1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
     1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
    -1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,

    -1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 1.0f, //front face
     1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
     1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
     1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
    -1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,

    -1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, //left face
    -1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
    -1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
    -1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f,

     1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f, //right face
     1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
     1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
     1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
     1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.0f,
     1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,

    -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 1.0f, //bottom face
     1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 1.0f,
     1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
     1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
    -1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 1.0f,

    -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 1.0f,
     1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 1.0f,
     1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,
     1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,
    -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,
    -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 1.0f


    };


    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // this is the variables where you can input the start day and how much you want to increment by framerate
    float day = 0.0f;
    float inc = 1.0f / 240;


    // render loop

    // -----------
    while (!glfwWindowShouldClose(window))
    {

        // input
        // -----
        processInput(window);
        day += inc;
        //sun angle values based on day
        float sunself = get_sun_rotate_angle_around_itself(day);
        //earth angle values based on day
        float earthsun = get_earth_rotate_angle_around_sun(day);
        float earthself = get_earth_rotate_angle_around_itself(day);
        //moon angle values based on day
        float moonearth = get_moon_rotate_angle_around_earth(day);
        float moonself = get_moon_rotate_angle_around_itself(day);

        // render
        // ------
        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!


        // activate shader
        ourShader.use();
        
        // create transformations
        glm::mat4 projection = glm::perspective(glm::radians(30.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        ourShader.setMat4("projection", projection);
        // view is 

        

       // // SUN

        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        //translate sun to center
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        //scale to 12x12x12
        model = glm::scale(model, glm::vec3(6.0f, 6.0f, 6.0f));

        //roation angle around itself
        float angleSSelf = sunself;
        model = glm::rotate(model, glm::radians(angleSSelf), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
       
        float earthOrbitRadius1 = 24.0f;
        float earthX1 = earthOrbitRadius1 * cos(glm::radians(earthsun));
        float earthZ1 = earthOrbitRadius1 * sin(glm::radians(-earthsun));

        // EARTH

        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
       
        // move earth to (24,0,0)
        float angleESun = earthsun;
        model = glm::rotate(model, glm::radians(angleESun), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(24.0f, 0.0f, 0.0f));

        //// Maintain tilt of 23.4 degrees during the orbit
        float tiltAngle = -23.4f;
        model = glm::rotate(model, glm::radians(tiltAngle), glm::vec3(0.0f, 0.0f, 1.0f));

        // scale earth to 6x6x6
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));

        // rotation around itself
        float angleESelf = earthself;
        model = glm::rotate(model, glm::radians(angleESelf), glm::vec3(0.0f, 1.0f, 0.0f));

        // rotation around the sun
        // angle = earthsun;
        // model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

        ourShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);




       // // MOON

       // model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
       // // translate to (36,0,0)
       // float angleMEarth = moonearth;

       // model = glm::translate(model, glm::vec3(23.9f*cos(moonearth), 0.0f, 0.1f*sin(moonearth)));
       // model = glm::rotate(model, glm::radians(moonearth), glm::vec3(0.0f, 1.0f, 0.0f));
       // model = glm::translate(model, glm::vec3(12.0f, 0.0f, 0.0f));

       // // scale to 3x3x3
       // model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
       //// rotation around self
       // float angleMSelf = moonself;
       // //model = glm::rotate(model, glm::radians(angleMSelf), glm::vec3(0.0f, 1.0f, 0.0f));
       // //// rotiaon around earth

       // ourShader.setMat4("model", model);
       // glDrawArrays(GL_TRIANGLES, 0, 36);
        // MOON

        model = glm::mat4(1.0f);
        // Calculate the current position of the earth around the sun
        float earthOrbitRadius = 24.0f;
        float earthX = earthOrbitRadius * cos(glm::radians(earthsun));
        float earthZ = earthOrbitRadius * sin(glm::radians(-earthsun));

        // Position the moon relative to the earth
        float moonOrbitRadius = 12.0f;
        float moonX = earthX + moonOrbitRadius * cos(glm::radians(moonearth)); // Adjusted to stay close to the earth
        float moonZ = earthZ + moonOrbitRadius * sin(glm::radians(-moonearth)); // Adjusted to stay close to the earth
        model = glm::translate(model, glm::vec3(moonX, 0.0f, moonZ));

        // Rotate the moon around the earth
        model = glm::rotate(model, glm::radians(moonearth), glm::vec3(0.0f, 1.0f, 0.0f)); // Same direction as earth
        // Scale the moon
        model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));

        // Ensure tidal lock between moon's rotation and its orbital period
        float moonRotation = moonself / normalize; // Match the moon's rotation with its orbital period
        model = glm::rotate(model, glm::radians(moonRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        
        glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), cameraUp);
        //glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f,0.0f,0.0f), cameraUp);

        ourShader.setMat4("view", view);
        glBindVertexArray(VAO);
        ourShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //glm::vec3 scale[]{
        //    glm::vec3(6.0f,6.0f,6.0f):
        //    glm::
        //};
        //for (unsigned int i = 0; i < 3; i++)
        //{
        //    // calculate the model matrix for each object and pass it to shader before drawing
        //    glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        //   
        //    model = glm::translate(model, cubePositions[i]);
        //  model = glm::scale(model, scale[i]);
        //    float angle = 0.0f;
        //    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
        //    ourShader.setMat4("model", model);

        //    glDrawArrays(GL_TRIANGLES, 0, 36);
        //}

        

        // render boxes
        //glBindVertexArray(VAO);
        //for (unsigned int i = 0; i < 10; i++)
        //{
        //    // calculate the model matrix for each object and pass it to shader before drawing
        //    glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        //    model = glm::translate(model, cubePositions[i]);
        //    float angle = -45.0f;
        //    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
        //    ourShader.setMat4("model", model);

        //    glDrawArrays(GL_TRIANGLES, 0, 36);
        //}


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    //press p to capture screen
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        std::cout << "Capture Window " << ss_id << std::endl;
        int buffer_width, buffer_height;
        glfwGetFramebufferSize(window, &buffer_width, &buffer_height);
        dump_framebuffer_to_ppm("Assignment0-ss", buffer_width, buffer_height);
    }
    float cameraSpeed = static_cast<float>(25 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}