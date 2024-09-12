#include "fiber.h"
#include <vector>
#include <list>

using namespace sylar; 

class Scheduler
{
public:
	// 添加协程调度任务
	void schedule(const std::shared_ptr<Fiber>& task)
	{
		m_active_tasks.push_back(std::move(task));
	}

	// 执行调度任务
	void run()
	{
		while(!m_active_tasks.empty()){
			auto it = m_active_tasks.front();
			m_active_tasks.pop_front();
			it->resume();
			if(it->getState() == Fiber::TERM){
				m_term_tasks.push_back(it);
			} else if(it->getState() == Fiber::READY) {
				m_active_tasks.push_back(it);
			} else {
				std::cout << "unright state: "  + it->getId() << std::endl;
			}
		}
		m_term_tasks.clear();
	}

private:
	// 任务队列
	std::list<std::shared_ptr<Fiber>> m_active_tasks;
	std::vector<std::shared_ptr<Fiber>> m_term_tasks;
};

void test_fiber(int i)
{
	std::cout << "pre hello world " << i << std::endl;
	std::shared_ptr<Fiber> cur = Fiber::GetThis();
	cur->yield();
	std::cout << "post hello world "<< i << std::endl;
}

int main()
{
	// 初始化当前线程的主协程
	Fiber::GetThis();

	// 创建调度器
	Scheduler sc;

	// 添加调度任务（任务和子协程绑定）
	for(auto i=0;i<20;i++)
	{
		// 创建子协程
			// 使用共享指针自动管理资源 -> 过期后自动释放子协程创建的资源
			// bind函数 -> 绑定函数和参数用来返回一个函数对象
		std::shared_ptr<Fiber> fiber = std::make_shared<Fiber>(std::bind(test_fiber, i), 0, false);
		sc.schedule(fiber);
	}

	// 执行调度任务
	sc.run();

	return 0;
}