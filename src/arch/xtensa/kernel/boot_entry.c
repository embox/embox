extern void kernel_start();

void call_user_start()
{
    kernel_start();
}


void die()
{
    while (1)
    {
    }
}
