#include <stdio.h>
#include <lightship/plugin_manager.h>
#include <lightship/vector.h>

int main(int argc, char** argv)
{
    vector_t* test = vector_create(sizeof(int));
    int x=5, y=6, z=12;
    vector_push(test, &x);
    vector_push(test, &y);
    vector_push(test, &z);
    
    printf("%d, %d, %d\n",
        *((int*)vector_get_element(test, 0)),
        *((int*)vector_get_element(test, 1)),
        *((int*)vector_get_element(test, 2))
    );
    
    int w=20;
    vector_insert(test, 1, &w);
    printf("%d, %d, %d, %d\n",
        *((int*)vector_get_element(test, 0)),
        *((int*)vector_get_element(test, 1)),
        *((int*)vector_get_element(test, 2)),
        *((int*)vector_get_element(test, 3))
    );
    
    vector_erase(test, 2);
    printf("%d, %d, %d\n",
        *((int*)vector_get_element(test, 0)),
        *((int*)vector_get_element(test, 1)),
        *((int*)vector_get_element(test, 2))
    );
    
    vector_clear(test);
    printf("%d\n", vector_count(test));
    
    return 0;
}