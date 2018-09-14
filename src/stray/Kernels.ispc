export void
render(uniform float v[],
       uniform int N)
{
    foreach(i = 0 ... N)
    {
        v[i] = 0.0f;
    }
}