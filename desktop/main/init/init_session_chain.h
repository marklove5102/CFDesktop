/**
 * @file init_session_chain.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Init Session Chains - 基于DAG依赖图的初始化阶段执行链
 * @version 0.1
 * @date 2026-03-25
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include "base/weak_ptr/weak_ptr.h"
#include "base/weak_ptr/weak_ptr_factory.h"
#include "boot_stage_info.h"
#include "init_stage.h"
#include <QObject>
#include <list>
#include <memory>
#include <vector>

namespace cf::desktop::init_session {

/**
 * @brief 初始化会话链，管理多个IInitStage的执行
 *
 * 功能特性：
 * - 基于DAG自动解析依赖关系并拓扑排序
 * - 检测循环依赖并报错
 * - OK/PartialFailed继续执行，CriticalFailed立即停止
 * - 提供详细的执行调试信息
 */
class InitSessionChain : public QObject {
    Q_OBJECT
  public:
    InitSessionChain(QObject* parent = nullptr);
    ~InitSessionChain();

    /**
     * @brief 获取全局Chain的WeakPtr引用（单例模式）
     */
    static cf::WeakPtr<InitSessionChain> GetChainRef();

    /**
     * @brief 添加一个初始化阶段到链尾部
     * @param[in] stage 唯一指针管理的Stage对象
     */
    void add_stage_back(std::unique_ptr<IInitStage> stage);

    /**
     * @brief 执行初始化链
     * @return 最终执行结果（OK/PartialFailed/CriticalFailed）
     *
     * 执行逻辑：
     * 1. 解析依赖图并生成拓扑排序
     * 2. 按顺序执行各Stage
     * 3. 遇到CriticalFailed立即停止并返回
     * 4. PartialFailed记录但继续执行
     */
    IInitStage::StageResult execute();

    /**
     * @brief 获取依赖图的调试字符串（用于调试和日志）
     * @return 描述依赖关系的可读字符串
     */
    QString get_dependency_graph_debug_string() const;

    /**
     * @brief 重置链状态，允许重新执行
     */
    void reset();

    /**
     * @brief 获取链中的Stage数量
     * @return Stage数量
     */
    size_t size() const { return execution_order.size(); }

  signals:
    /**
     * @brief Stage开始执行信号
     */
    void stage_start(const BootStageInfo& bs_info);

    /**
     * @brief Stage完成执行信号
     * @param[in] stage_name Stage名称
     * @param[in] success 是否成功（包括部分成功）
     */
    void stage_completed(const QString& stage_name, bool success);

    /**
     * @brief Stage部分失败信号
     * @param[in] stage_name Stage名称
     * @param[in] message 失败消息
     * @param[in] detail 错误详情
     */
    void stage_partial_failed(const QString& stage_name, const QString& message,
                              const QString& detail);

    /**
     * @brief Chain执行完成信号
     * @param[in] final_result 最终结果
     */
    void chain_finished(const IInitStage::StageResult& final_result);

  private:
    /**
     * @brief 依赖图节点，用于拓扑排序
     */
    struct GraphNode {
        IInitStage* stage;                     // Stage指针
        std::vector<IInitStage*> dependencies; // 依赖的Stage列表
        std::string stage_name;                // Stage名称（用于调试）
        int in_degree;                         // 入度（用于Kahn算法）
    };

    /**
     * @brief 构建依赖图
     * @return 成功返回排序后的Stage列表，失败返回空（循环依赖）
     */
    std::vector<GraphNode> build_dependency_graph();

    /**
     * @brief 拓扑排序（Kahn算法）
     * @param graph 依赖图
     * @return 排序后的Stage指针列表
     */
    std::vector<IInitStage*> topological_sort(std::vector<GraphNode>& graph);

    /**
     * @brief 检测循环依赖
     * @param graph 依赖图
     * @return 存在循环依赖返回true
     */
    bool has_circular_dependency(const std::vector<GraphNode>& graph) const;

    /**
     * @brief 生成依赖图调试字符串
     * @param graph 依赖图
     * @return 格式化的调试信息字符串
     */
    QString generate_debug_string(const std::vector<GraphNode>& graph) const;

  private:
    std::list<std::unique_ptr<IInitStage>> stages;
    std::vector<IInitStage*> execution_order; // 拓扑排序后的执行顺序
    QString last_debug_string;                // 上次的依赖图调试字符串
    bool has_run{false};
    bool is_valid{true}; // 依赖图是否有效（无循环依赖）

    cf::WeakPtrFactory<InitSessionChain> weak_ptr_factory_;
};

} // namespace cf::desktop::init_session
